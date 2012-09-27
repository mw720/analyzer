//! \file V792.hxx
//! \author G. Christian
//! \brief Defines classes relevant to unpacking data from CAEN ADC modules V792 and V785.
#ifndef  DRAGON_VME_ADC_HXX
#define  DRAGON_VME_ADC_HXX
#include <stdint.h>
#include "utils/Error.hxx"
#include "Constants.hxx"

namespace midas { class Event; }

namespace vme {

/// Encloses CAEN vme module classes
namespace caen {

/// CAEN v792 ADC
class V792 {
public:
	static const uint16_t DATA_BITS    = 0x0; ///< Code to specify a data buffer
	static const uint16_t HEADER_BITS  = 0x2; ///< Code to specify a header buffer
	static const uint16_t FOOTER_BITS  = 0x4; ///< Code to specify a footer buffer
	static const uint16_t INVALID_BITS = 0x6; ///< Code to specify an invalid buffer
	static const uint16_t MAX_CHANNELS = 32; 	///< Number of data channels availible in the ADC

	int16_t n_ch;	  ///< Number of channels present in an event

	int32_t count;  ///< Event counter

	bool overflow; 	///< Is any channel an overflow?

	bool underflow; ///< Is any channel under threshold?

	int16_t data[MAX_CHANNELS]; ///< Array of event data

public:
	/// Unpack ADC data from a midas event
	bool unpack(const midas::Event& event, const char* bankName, bool reportMissing = false);

	/// Reset data fields to default values
	void reset();

	/// Get a data value, with bounds checking
	int16_t get_data(uint16_t ch) const
		{
			/*!
			 * \param ch Channel number to get data from
			 * Returns the data value stored at \e ch. If \e ch is out of bounds
			 * of the internal array, prints a warning message and returns vme::NONE.
			 */
			if (ch > 0 && ch < MAX_CHANNELS) return data [ch];
			else {
				err::Warning("V792::get_data")
					<< "Channel number " << ch << "out of bounds (valid range: [0, "
					<< MAX_CHANNELS -1 << "]\n";
				return vme::NONE;
			}
		}

	/// Calls reset(),
	V792() { reset(); }

	/// Empty
	~V792() { }

private:
  /// Unpack event data from a caen 32 channel adc
	bool unpack_data_buffer(const uint32_t* const pbuffer);

  /// Unpack a Midas data buffer from a CAEN ADC
	bool unpack_buffer(const uint32_t* const pbuffer, const char* bankName);

}; // struct V792


/// Readout structure of CAEN V785 ADC (peak sensing) is identical to V792 QDC (charge sensing).
typedef V792 V785;

} // namespace caen

} // namespace vme




#endif
