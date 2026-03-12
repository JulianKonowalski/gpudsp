#include "wav/WAVReader.hpp"

#include <cstring>
#include <fstream>

using namespace gpudsp::wav;

WAVReader::WAVReader(const std::string& filepath) {
	std::ifstream wav_file(filepath, std::ios::binary);
	if (!wav_file.is_open()) { throw std::runtime_error("Failed to open file: " + filepath); }

	char check[5];
	check[4] = '\0';
	
	/* MASTER CHUNK */

	wav_file.read(check, 4);
	if (strcmp(check, "RIFF")) { throw std::runtime_error("Invalid WAV file format."); }

	wav_file.read((char*)&m_wav_specification.file_size, sizeof(uint32_t));

	wav_file.read(check, 4);
	if (strcmp(check, "WAVE")) { throw std::runtime_error("Invalid WAV file format."); }

	/* FORMAT CHUNK */

	wav_file.read(check, 4);
	if (strcmp(check, "fmt ")) { throw std::runtime_error("Invalid WAV file format."); }

	uint32_t format_chunk_size;
	wav_file.read((char*)&format_chunk_size, sizeof(uint32_t));

	uint16_t audio_format;
	wav_file.read((char*)&audio_format, sizeof(uint16_t));
	if (audio_format != 1) { throw std::runtime_error("Unsupported WAV file format."); }

	wav_file.read((char*)&m_wav_specification.num_channles, sizeof(uint16_t));
	wav_file.read((char*)&m_wav_specification.sample_rate, sizeof(uint32_t));
	wav_file.read((char*)&m_wav_specification.bytes_per_second, sizeof(uint32_t));
	wav_file.read((char*)&m_wav_specification.bytes_per_block, sizeof(uint16_t));
	wav_file.read((char*)&m_wav_specification.bits_per_sample, sizeof(uint16_t));

	/* GARBAGE */

	wav_file.read(check, 4);
	while (strcmp(check, "data")) { //ignore other chunks
		uint32_t chunk_size;
		wav_file.read((char*)&chunk_size, sizeof(uint32_t));
		for (uint32_t i = 0; i < chunk_size; ++i) { wav_file.read(check, 1); }
		wav_file.read(check, 4);
	}

	/* DATA CHUNK */

	wav_file.read((char*)&m_wav_specification.data_size, sizeof(uint32_t));

	m_wav_specification.num_samples = m_wav_specification.data_size / m_wav_specification.bytes_per_block;
	m_samples = new float[m_wav_specification.num_channles * m_wav_specification.num_samples];

	uint32_t sample;
	uint32_t max_sample = ((0b1 << (m_wav_specification.bits_per_sample - 1)) - 1);
	for (uint32_t sample_id = 0; sample_id < m_wav_specification.num_samples; ++sample_id) {
		for (uint16_t channel_id = 0; channel_id < m_wav_specification.num_channles; ++channel_id) {
			sample = 0;
			for (uint8_t k = 0; k < m_wav_specification.bits_per_sample / 8; ++k) {
				sample <<= 8;
				uint8_t byte = 0;
				wav_file.read((char*)&byte, sizeof(uint8_t));
				sample |= byte;
			}
			m_samples[(2 * sample_id) + channel_id] = ((double)sample / max_sample) - 1.0;
		}
	}
}

WAVReader::~WAVReader(void) { delete m_samples; }