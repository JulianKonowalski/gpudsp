#ifndef DATASTRUCT_RINGBUFFER_HPP
#define DATASTRUCT_RINGBUFFER_HPP
#pragma once

#include <mutex>
#include <cstring>

namespace gpudsp::datastruct {

/*----------------------------------------------------------------------------*/

template <typename T>
class RingBuffer {
public:

    RingBuffer(
        const size_t chunk_size,
        const size_t num_chunks
    ) : m_buffer(new T[chunk_size * num_chunks]),
        m_data_end_idx(0),
        m_data_begin_idx(0),
        m_chunk_size(chunk_size),
        m_num_chunks(num_chunks),
        m_looped(false) {}
    ~RingBuffer(void) { delete m_buffer; }

    inline size_t getChunkSize(void) const { return m_chunk_size; }
    inline size_t getNumChunks(void) const { return m_num_chunks; }

    bool readable(void) { return m_looped || m_data_begin_idx != m_data_end_idx; }
    bool writeable(void) { return !m_looped; }

    void readChunk(T* output_buffer) {
        if (!this->readable()) { return; }
        std::lock_guard<std::mutex> lock(m_mutex);
        std::memcpy(output_buffer, &m_buffer[m_data_begin_idx * m_chunk_size], m_chunk_size * sizeof(T));
        m_data_begin_idx = (m_data_begin_idx + 1) % m_num_chunks;
        m_looped = false;
    }

    void writeChunk(T* input_buffer) {
        if (!this->writeable()) { return; }
        std::lock_guard<std::mutex> lock(m_mutex);
        std::memcpy(&m_buffer[m_data_end_idx * m_chunk_size], input_buffer, m_chunk_size * sizeof(T));
        m_data_end_idx = (m_data_end_idx + 1) % m_num_chunks;
        if (m_data_begin_idx == m_data_end_idx) { m_looped = true; }
    }

private:

    std::mutex m_mutex;
    T* m_buffer;
    size_t m_data_end_idx;
    size_t m_data_begin_idx;
    const size_t m_chunk_size;
    const size_t m_num_chunks;
    bool m_looped;

};

/*----------------------------------------------------------------------------*/

}


#endif