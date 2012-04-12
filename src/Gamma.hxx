/// \file Gamma.hxx
/// \brief Defines classes for DRAGON head (gamma) detectors.
#ifndef DRAGON_GAMMA_HXX
#define DRAGON_GAMMA_HXX
#include "Modules.hxx"

namespace dragon {

namespace gamma {

/// The BGO array
class Bgo {
public:
// ==== Const Data ==== //
	 /// Number of channels in the BGO array
	 static const int nch = 30; //!

// ==== Classes ==== //	 
	 /// Bgo variables
	 class Variables {
	 public:
      /// \brief Maps ADC channel to BGO detector
			/// \details Example: setting ch[0] = 12, means that the 0th detector
			/// in the BGO array reads its charge data from channel 12 of the qdc.
			int qdc_ch[Bgo::nch];

      /// \brief Maps TDC channel to BGO detector
			/// \details Similar to qdc_ch
			int tdc_ch[Bgo::nch];

			/// Constructor, sets *_ch[i] to i
			Variables();

			/// Destructor, nothing to do
			~Variables() { }

      /// \brief Set variable values from an ODB file
			/// \param [in] odb_file Path of the odb file from which you are extracting variable values
			/// \todo Needs to be implemented once ODB is set up
			void set(const char* odb_file);
	 };

// ==== Data ==== //
	 /// Instance of Bgo::Variables for mapping digitizer ch -> bgo detector
	 Variables variables; //!
	 
	 /// Raw charge signals, per detector
	 int16_t q[Bgo::nch];    //#

   /// Raw timing signals, per detector
	 int16_t t[Bgo::nch];    //#

// ==== Methods ==== //	 
	 /// Constructor, initializes data values
	 Bgo();

	 /// Destructor, nothing to do
	 ~Bgo() { }

   /// Sets all data values to vme::NONE
	 void reset();

	 /// Read data from a midas event.
	 /// \param modules Electronic modules structure from which the data are read
	 void read_data(const dragon::gamma::Modules& modules);
};

/// All DRAGON gamma ray detectors
/*! Currently this is just the BGO array, but in the future we may add others
 *  (germanium, BaF, etc.) */
class Gamma {
public:
// ==== Data ==== //
	 /// Gamma electronics modules
	 dragon::gamma::Modules modules; //#

   /// Event counter
	 int32_t evt_count; //#

// ==== Detectors ==== //
	 /// Bgo array
	 Bgo bgo;

// ==== Methods ==== //	 
	 /// Constructor, initializes data values
	 Gamma();

	 /// Destructor, nothing to do
	 ~Gamma() { }

   /// Sets all data values to vme::NONE
	 void reset();

	 /// Unpack midas event data
	 /// \param [in] event Reference to a Midas event structure
	 void unpack(TMidasEvent& event);

	 /// \brief Read data from modules into detector classes
	 void read_data();
};
	 

} // namespace gamma

} // namespace dragon


#endif