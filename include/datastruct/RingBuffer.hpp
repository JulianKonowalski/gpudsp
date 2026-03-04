#ifndef DATASTRUCT_RINGBUFFER_HPP
#define DATASTRUCT_RINGBUFFER_HPP
#pragma once

#include <array>
#include <mutex>
#include <cstring>

namespace gpudsp::datastruct {

/*----------------------------------------------------------------------------*/

template <typename T, size_t CHUNK_SIZE, size_t NUM_CHUNKS>
class RingBuffer {
public:

    RingBuffer(void) : m_data_end_idx(0), m_data_begin_idx(0), m_looped(false) {}
    ~RingBuffer(void) = default;

    inline constexpr size_t getChunkSize(void) { return CHUNK_SIZE; }
    inline constexpr size_t getNumChunks(void) { return NUM_CHUNKS; }

    bool readable(void) { return m_looped || m_data_begin_idx != m_data_end_idx; }
    bool writeable(void) { return !m_looped; }

    void readChunk(T* output_buffer) {
        if (!this->readable()) { return; }
        std::lock_guard<std::mutex> lock(m_mutex);
        std::memcpy(output_buffer, m_buffer[m_data_begin_idx].data(), CHUNK_SIZE * sizeof(T));
        m_data_begin_idx = (m_data_begin_idx + 1) % NUM_CHUNKS;
        m_looped = false;
    }

    void writeChunk(T* input_buffer) {
        if (!this->writeable()) { return; }
        std::lock_guard<std::mutex> lock(m_mutex);
        std::memcpy(m_buffer[m_data_end_idx].data(), input_buffer, CHUNK_SIZE * sizeof(T));
        m_data_end_idx = (m_data_end_idx + 1) % NUM_CHUNKS;
        if (m_data_begin_idx == m_data_end_idx) { m_looped = true; }
    }

private:
    
    std::mutex m_mutex;
    std::array<std::array<T, CHUNK_SIZE>, NUM_CHUNKS> m_buffer;
    size_t m_data_end_idx;
    size_t m_data_begin_idx;
    bool m_looped;

};

/*----------------------------------------------------------------------------*/

}


#endif