#pragma once

#include <string>
#include <cstdint>
#include <fstream>

namespace gpudsp::wav {

/*----------------------------------------------------------------------------*/

class WAVReader final {
public:

	struct WAVSpecification {
		uint32_t file_size			= 0;
		uint32_t sample_rate		= 0;
		uint32_t data_size			= 0;
		uint32_t bytes_per_second	= 0;
		uint32_t num_samples		= 0;
		uint16_t bytes_per_block	= 0;
		uint16_t bits_per_sample	= 0;
		uint16_t num_channles		= 0;
	};

	WAVReader(const std::string& filepath);
	~WAVReader(void);

	inline WAVSpecification getSpecificaiton(void) const { return m_wav_specification; }
	inline const float* getSamples(void) { return m_samples; }

private:

	WAVSpecification m_wav_specification;
	float* m_samples;

};

/*----------------------------------------------------------------------------*/

}