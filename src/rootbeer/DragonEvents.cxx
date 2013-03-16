/// \file DragonEvents.cxx
/// \author G. Christian
/// \brief Implements DragonEvents.hxx
#include <cassert>
#include "utils/ErrorDragon.hxx"
#include "midas/Database.hxx"
#include "DragonEvents.hxx"


namespace {

template <class T, class E>
inline void handle_event(rb::data::Wrapper<T>& data, const E* buf)
{
	data->reset();
	data->unpack(*buf);
	data->calculate();
}

} // namespace


// ======== Class rootbeer::RunParameters ======== //

rootbeer::RunParameters::RunParameters():
	fParameters("runpar", this, false, "") { }

Bool_t rootbeer::RunParameters::DoProcess(const void* addr, Int_t)
{
	const midas::Database* pdb = reinterpret_cast<const midas::Database*> (addr);
	fParameters->read_data(pdb);
	return kTRUE;
}

void rootbeer::RunParameters::HandleBadEvent()
{
	dragon::utils::err::Error("RunParameters")
		<< "Unknown error encountered during event processing";
}


// ======== Class rootbeer::TStampDiagnostics ======== //

rootbeer::TStampDiagnostics::TStampDiagnostics():
	fDiagnostics("tstamp", this, false, "") { }

void rootbeer::TStampDiagnostics::HandleBadEvent()
{
	dragon::utils::err::Error("TStampDiagnostics")
		<< "Unknown error encountered during event processing";
}

tstamp::Diagnostics* rootbeer::TStampDiagnostics::GetDiagnostics()
{
	/*!
	 * \warning Returns a pointer to an instance of data shared between threads!
	 * Ensure this is only used in cases where protected by a mutex lock.
	 */
	return fDiagnostics.Get();
}


// ======== Class rootbeer::GammaEvent ======== //

rootbeer::GammaEvent::GammaEvent():
	fGamma("gamma", this, true, "") { }

void rootbeer::GammaEvent::HandleBadEvent()
{
	dragon::utils::err::Error("GammaEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::GammaEvent::DoProcess(const void* addr, Int_t nchar)
{
  /*!
	 * Calls unpacking routines of dragon::Head to extract the raw MIDAS
	 * data into a dragon::Head structure. Then calculates higher-level
	 * parameters.
	 */
	if(!addr) {
		dragon::utils::err::Error("rootbeer::GammaEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fGamma, AsMidasEvent(addr));

	return true;
}

void rootbeer::GammaEvent::ReadVariables(const char* dbname)
{
	fGamma->set_variables(dbname);
}


// ======== Class rootbeer::HeavyIonEvent ======== //

rootbeer::HeavyIonEvent::HeavyIonEvent():
	fHeavyIon("hi", this, true, "") { }

void rootbeer::HeavyIonEvent::HandleBadEvent()
{
	dragon::utils::err::Error("HeavyIonEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::HeavyIonEvent::DoProcess(const void* addr, Int_t nchar)
{
  /*!
	 * Calls unpacking routines of dragon::Tail to extract the raw MIDAS
	 * data into a dragon::Head structure. Then calculates higher-level
	 * parameters.
	 */
	if(!addr) {
		dragon::utils::err::Error("rootbeer::HeavyIonEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fHeavyIon, AsMidasEvent(addr));
	return true;
}

void rootbeer::HeavyIonEvent::ReadVariables(const char* dbname)
{
		fHeavyIon->set_variables(dbname);
}


// ======== Class rootbeer::CoincEvent ======== //

rootbeer::CoincEvent::CoincEvent():
	fCoinc("coinc", this, false, "") { }

void rootbeer::CoincEvent::HandleBadEvent()
{
	dragon::utils::err::Error("CoincEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::CoincEvent::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks data from the head and tail MIDAS events into the corresponding
	 * fields of fDragon, then calls the calculate() methods of each.
	 * \todo Figure out a way to handle this without unpacking the events; in principle,
	 * they should have already been handled as singles events, thus we add extra overhead
	 * by going through the unpacking routines twice - it should be possible to buffer and
	 * then copy the already-unpacked head and tail structures directly.
	 */
	if(!addr) {
		dragon::utils::err::Error("rootbeer::CoincEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fCoinc, AsCoincMidasEvent(addr));
	return true;
}

void rootbeer::CoincEvent::ReadVariables(const char* dbname)
{
	fCoinc->set_variables(dbname);
}

// ======== Class rootbeer::HeadScaler ======== //

rootbeer::HeadScaler::HeadScaler():
	fScaler("head_scaler", this, true)
{
	assert(fScaler.Get());
	fScaler->variables.set_bank_names("SCH");
}

void rootbeer::HeadScaler::HandleBadEvent()
{
	dragon::utils::err::Error("HeadScaler")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::HeadScaler::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks into scaler event structure.
	 * \returns true is given a valid \d addr input, false otherwise
	 */
	if(!addr) {
		dragon::utils::err::Error("rootbeer::HeadScaler::DoProcess") << "Received NULL event address";
		return false;
	}

	fScaler->unpack(*AsMidasEvent(addr));
	return true;
}

void rootbeer::HeadScaler::ReadVariables(const char* dbname)
{
	fScaler->set_variables(dbname, "head");
}


// ======== Class rootbeer::TailScaler ======== //

rootbeer::TailScaler::TailScaler():
	fScaler("head_scaler", this, true)
{
	assert(fScaler.Get());
	fScaler->variables.set_bank_names("SCT");
}

void rootbeer::TailScaler::HandleBadEvent()
{
	dragon::utils::err::Error("TailScaler")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::TailScaler::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks into scaler event structure.
	 * \returns true is given a valid \d addr input, false otherwise
	 */
	if(!addr) {
		dragon::utils::err::Error("rootbeer::TailScaler::DoProcess") << "Received NULL event address";
		return false;
	}

	fScaler->unpack(*AsMidasEvent(addr));
	return true;
}

void rootbeer::TailScaler::ReadVariables(const char* dbname)
{
	fScaler->set_variables(dbname, "tail");
}
