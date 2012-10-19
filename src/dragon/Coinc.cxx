/// \file Coinc.cxx
/// \author G. Christian
/// \brief Implements Coinc.hxx
#include "utils/Valid.hxx"
#include "Coinc.hxx"


// ========== Class dragon::Coinc ========== //

dragon::Coinc::Coinc():
	xtof(dragon::NO_DATA), head(), tail()
{
	reset();
}

dragon::Coinc::Coinc(const dragon::Head& head, const dragon::Tail& tail):
	xtof(dragon::NO_DATA)
{
	read_event(head, tail);
}

void dragon::Coinc::reset()
{
	head.reset();
	tail.reset();
	reset_data(xtof);
}

void dragon::Coinc::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file (*.xml or *.mid) from which to read
	 * \note Passing \c "online" looks at the experiment's ODB, if connected
	 */
	head.set_variables(odb);
	tail.set_variables(odb);
}

void dragon::Coinc::read_event(const dragon::Head& head_, const dragon::Tail& tail_)
{
	/*!
	 * \param [in] head_ Head (gamma-ray) event
	 * \param [in] tail_ Tail (heavy-ion) event
	 */
	head = head_;
	tail = tail_;
}

void dragon::Coinc::unpack(const midas::CoincEvent& coincEvent)
{
	head.unpack( *(coincEvent.fGamma) );
	tail.unpack( *(coincEvent.fHeavyIon) );
}

void dragon::Coinc::calculate_coinc()
{
	/// \todo Implement dragon::Coinc::calculate() properly
	xtof = dragon::NO_DATA ; // head - tail //
}	

void dragon::Coinc::calculate()
{
	/*!
	 * Does calculation for internal head and tail fields first,
	 * then calculates coincidence parameters.
	 */
	head.calculate();
	tail.calculate();
	calculate_coinc();
}































// DOXYGEN MAINPAGE //

/*!
	\mainpage
	\authors G. Christian
	\authors C. Stanford
	\authors K. Olchanski

	\section intro Introduction

	This software package is developed for the analysis of data collected in DRAGON experiments
	at TRIUMF. The software is intended for experiments run with the "new" (c. 2013) DRAGON data-acquisition
	system (VME hardware + timestamp coincidence matching).

	At it's core, the package is simply a collection of routines to extract ("unpack") the
	<a href=http://midas.psi.ch>MIDAS</a> data
	generated during an experiment into a format that is more suitable for human consumption, as well as to
	perform	some of the more basic and commonly needed analysis tasks. From here, users can further process the
	data using their own codes, or they can interface with a vistualzation library to view the data in histograms,
	either online or offline. The core of the package is written in "plain" C++, that is without any dependence on
	libraries other than the standard library.

	In addition to the "core", this package also includes code to interface with the
	<a href=http://ladd00.triumf.ca/~olchansk/rootana/>rootana</a> data analysis package distributed as part of
	<a href=http://midas.psi.ch>MIDAS</a> and the
	<a href=http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/index.html>rootbeer</a> program for data
	vistualzation. Each of these packages is designed for "user friendly" viewing of data in histograms, either
	from saved files or real-time online data. Both require that a relatively recent
	version of <a href=http://root.cern.ch>ROOT</a> be installed on your system, and, additionally,
	<a href=http://midas.psi.ch> MIDAS</a> must be installed in order to look at online data (offline
	data viewing should be possible without a MIDAS installation, however).
	
	\todo There are plans to package the DRAGON analysis classes as a python module, allowing data to be analyzed
	in python independent of ROOT/PyRoot. Stay tuned....



	\section installation Installation
	
	\subsection dependencies Dependencies
	For the "core" functionality, all that you will need is a working C++ compiler; there is no dependence on
	any third-party libraries. The optional rootbeer or rootana extensions each require ROOT to be installed
	(and, of course, the packages themselves). To look at online data, you will need MIDAS installed, and, if
	using the rootana system, <a href=https://daq-plone.triumf.ca/SR/ROODY>roody</a> is required for online
	histogram viewing.

	\section using For Users

	\section developers For Developers


 */
