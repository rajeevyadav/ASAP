#include "DocumentCache.h"

namespace ASAP
{
	DocumentCache::DocumentCache(void)
	{
	}

	size_t DocumentCache::LoadDocument(const boost::filesystem::path& filepath, const std::string& factory)
	{
		auto existing_entry = m_path_to_id_.find(filepath.string());
		if (existing_entry != m_path_to_id_.end())
		{
			return existing_entry->second;
		}

		auto result = m_documents_.insert({ m_document_counter_, std::shared_ptr<Document>(new Document(filepath, factory)) });
		m_instance_counters_.insert({ result.first->first, 0 });
		m_path_to_id_.insert({ filepath.string(), result.first->first });
		++m_document_counter_;
		return result.first->first;
	}

	void DocumentCache::UnloadDocument(const size_t id, const bool force)
	{
		if (m_documents_[id].use_count() == 1 || force)
		{
			m_instance_counters_.erase(id);
			m_documents_.erase(id);

			for (auto it = m_path_to_id_.begin(); it != m_path_to_id_.end(); ++it)
			{
				if (it->second == id)
				{
					m_path_to_id_.erase(it);
					break;
				}
			}
		}
	}

	DocumentInstance DocumentCache::GetDocument(const size_t id)
	{
		m_instance_counters_[id] += 1;
		return DocumentInstance(m_documents_[id], id, m_instance_counters_[id]);
	}

	DocumentInstance DocumentCache::GetDocument(const boost::filesystem::path& filepath)
	{
		size_t id = this->GetDocumentId(filepath);
		m_instance_counters_[id] += 1;
		return DocumentInstance(m_documents_[id], id, m_instance_counters_[id]);
	}

	size_t DocumentCache::GetDocumentId(const boost::filesystem::path& filepath)
	{
		return m_path_to_id_[filepath.string()];
	}
}