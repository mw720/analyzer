///
/// \file Event.hxx
/// \author G. Christian
/// \brief Defines an inherited class of TMidasEvent that includes
/// specific functionality for timestamp coincidence matching.
///
#ifndef DRAGON_MIDAS_EVENT_HXX
#define DRAGON_MIDAS_EVENT_HXX
#include <set>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cassert>
#include <cstring>
#include <typeinfo>
#include "midas/libMidasInterface/TMidasFile.h"
#include "midas/libMidasInterface/TMidasEvent.h"
#include "utils/ErrorDragon.hxx"


/// Enclodes dragon-specific midas classes
namespace midas {

/// Typedef for a MIDAS bank name
typedef char Bank_t[5];

/// Derived class of TMidasEvent for timestamped dragon events
/*!
 * Stores timestamp values as fields for easy access. Also provides
 * constructors to set an event from the addresses returned by polling.
 */
class Event: public TMidasEvent {

public:
	/// Provide typdef for EventHeader_t
	/*!
	 * \code
	 * struct EventHeader_t {
	 *   uint16_t fEventId;      // event id
	 *   uint16_t fTriggerMask;  // event trigger mask
	 *   uint32_t fSerialNumber; // event serial number
	 *   uint32_t fTimeStamp;    // event timestamp in seconds
	 *   uint32_t fDataSize;     // event size in bytes
	 *  };
	 * \endcode
	 */
	typedef TMidas_EVENT_HEADER Header;

	/// Number of fifo channels
	static const uint32_t MAX_FIFO = 4;

	/// FIFO channel w/ the trigger as input
	static const uint32_t TRIGGER_CHANNEL = 0;

private:
	/// Coincidence window (in us)
	double fCoincWindow;

	/// Timestamp value in clock cycles since BOR
	uint64_t fClock;

	/// TSC4 fifo values
	std::vector<uint64_t> fFifo[MAX_FIFO];

	/// Timestamp value in uSec
	double fTriggerTime;

public:
	/// Empty constructor
	Event(): TMidasEvent() { }

	/// Construct from event callback parameters, with TSC handling
	Event(const void* header, const void* data, int size, const Bank_t tsbank, double coinc_window);

	/// Construct from direct polling parameters, with TSC handling
	Event(char* buf, int size, const Bank_t tsbank, double coinc_window);

	/// Construct from event callback parameters, _without_ TSC handling
	Event(const void* header, const void* data, int size);

	/// Construct from direct polling parameters, _without_ TSC handling
	Event(char* buf, int size);

	/// Copy constructor
	Event(const Event& other) { CopyDerived(other); }

	/// Assignment operator
	Event& operator= (const Event& other)
		{ CopyDerived(other); return *this; }

	/// Copies event header information into another one
	void CopyHeader(Header& destination) const
		{	memcpy (&destination, &fEventHeader, sizeof(Header)); }

	/// Read an event from a TMidasFile
	bool ReadFromFile(TMidasFile& file)
		{	Clear(); return file.Read(this); }

	/// Returns trigger time in uSec
	double TriggerTime() const { return fTriggerTime; }

	/// Returns the trigger time in clock cycles
	uint64_t ClockTime() const { return fClock; }

	/// Copy fifo values to an external vector array
	void CopyFifo(std::vector<uint64_t>* pfifo) const;

	/// Checks if two events are coincident
	bool IsCoinc(const Event& other) const
		{ return fabs(TimeDiff(other)) < fCoincWindow; }

	/// Less than operator
	/*! \returns false if the two event's trigger times are within
	 *  the coincidence window; otherwise returns true if the trigger
	 *  time of 'this' is less than the trigger time of 'other' */
	bool operator< (const Event& rhs) const
		{
			if (IsCoinc(rhs)) return false;
			return TimeDiff(rhs) < 0.;
		}

	/// Prints timestamp information for a singles event
	void PrintSingle(FILE* where = stdout) const;

	/// Prints timestamp information for coincidence events
	void PrintCoinc(const Event& other, FILE* where = stdout) const;

	/// Calculates difference of timestamps
	double TimeDiff(const Event& other) const;

	/// Destructor, empty
	virtual ~Event() { }

	/// Bank finding routine (templated)
	template <typename T>
	T* GetBankPointer(const Bank_t name, int* length, bool reportMissing = false, bool checkType = false) const
		{
			/*!
			 * \param [in] name Name of the bank to search for
			 * \param [out] Length of the returned bank
			 * \param [in] reportMissing True means a warning message is printed if the bank is absent
			 * \param [in] checkType Specifies whether or not to check that the template parameter
			 *  matches the TID of the bank. If this parameter is set to true and the types do
			 *  not match, the error is fatal.
			 * \returns Pointer to the beginning of the bank
			 */
			void *pbk;
			int type;
			int bkfound = FindBank(name, length, &type, &pbk);

			if(!bkfound && reportMissing) {
				dragon::utils::Warning("midas::Event::GetBankPointer<T>", __FILE__, __LINE__)
					<< "Couldn't find the MIDAS bank \"" << name  << "\". Skipping...\n";
			}
			if (bkfound && checkType) {
				switch (type) {
				case 1:  assert (typeid(T) == typeid(unsigned char)); break; // TID_BYTE   1	
				case 2:  assert (typeid(T) == typeid(char));          break; // TID_SBYTE  2	
				case 3:  assert (typeid(T) == typeid(unsigned char)); break; // TID_CHAR   3	
				case 4:  assert (typeid(T) == typeid(uint16_t));      break; // TID_WORD   4	
				case 5:  assert (typeid(T) == typeid(int16_t));       break; // TID_SHORT  5	
				case 6:  assert (typeid(T) == typeid(uint32_t));      break; // TID_DWORD  6	
				case 7:  assert (typeid(T) == typeid(int32_t));       break; // TID_INT    7	
				case 8:  assert (typeid(T) == typeid(bool));          break; // TID_BOOL   8	
				case 9:  assert (typeid(T) == typeid(float));         break; // TID_FLOAT  9	
				case 10: assert (typeid(T) == typeid(double));        break; // TID_DOUBLE 10
				default:
					fprintf(stderr, "Unknown type id: %i\n", type);
					assert(false); break;
				}
			}
			return bkfound ? reinterpret_cast<T*>(pbk) : 0;
		}

private:
	/// Helper function for copy constructor / assignment operator
	void CopyDerived(const Event& other);

	/// Helper function for constructors
	void Init(const char* tsbank, const void* header, const void* addr, int size);

public:
	/// Class to compare by event id
	struct CompareId {
		/// Returns true if event id of lhs is less-than event id of rhs
		bool operator() (const Event& lhs, const Event& rhs) const
			{ return lhs.GetEventId() < rhs.GetEventId(); }
	};

	/// Class to compare by serial number
	struct CompareSerial {
		/// Returns true if serial number of lhs is less-than serial number of rhs
		bool operator() (const Event& lhs, const Event& rhs) const
			{ return lhs.GetSerialNumber() < rhs.GetSerialNumber(); }
	};

	/// Class to compare trigger times
	struct CompareTrigger {
		/// Parenthesis operator
		bool operator() (const Event& lhs, const Event& rhs) const
			{
				/*!
				 * \returns false if the two event's trigger times are within
				 * the coincidence window; otherwise returns true if the trigger
				 * time of lhs is less than the trigger time of rhs
				 * \note Same thing as <tt>lhs < rhs</tt>
				 */
				if (lhs.IsCoinc(rhs)) return false;
				return lhs.TimeDiff(rhs) < 0.; // fTriggerTime < rhs.fTriggerTime;
			}
	};

};

/// Simple struct to hold a dragon coincidence event
struct CoincEvent {

	const Event* fGamma; ///< Pointer to the head (gamma) event.

	const Event* fHeavyIon; ///< Pointer to the tail (heavy-ion) event.

	double xtrig; ///< Tail - Head trigger times (in microseconds), including rollover.

	/// Sets event pointers
	CoincEvent (const Event& event1, const Event& event2);
	
	/// Empty, no ownership of pointers
	~CoincEvent() { }

private:
	/// Disallow copy
	CoincEvent(const CoincEvent&) { }

	/// Disallow assign
	CoincEvent operator= (const CoincEvent) { return *this; }
};

} // namespace midas


#endif // #ifndef DRAGON_MIDAS_EVENT_HXX
