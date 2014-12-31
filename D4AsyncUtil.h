/*
 * D4AsyncUtil.h
 *
 *  Created on: Feb 18, 2014
 *      Author: ndp
 */

#ifndef D4ASYNCUTIL_H_
#define D4ASYNCUTIL_H_

#include "XMLWriter.h"

namespace libdap {


enum RejectReasonCode { TIME, UNAVAILABLE, PRIVILEGES, OTHER };


class D4AsyncUtil {
private:
#if 0
    // Not used
	string *d_stylesheet_ref;
#endif

public:
	D4AsyncUtil();
	virtual ~D4AsyncUtil();

	const static string STYLESHEET_REFERENCE_KEY;


	/**
	 * @brief Write the DAP4 AsyncRequired response.
	 * Print the AsyncRequired in XML form.
	 * @param xml Print to this XMLWriter instance
	 */
	void writeD4AsyncRequired(XMLWriter &xml, long expectedDelay, long responseLifetime, string *stylesheet_ref=0);


	/**
	 * @brief Write the DAP4 AsyncAccepted response.
	 * Write the AsyncAccepted in XML form.
	 * @param xml Print to this XMLWriter instance
	 */
	void writeD4AsyncAccepted(XMLWriter &xml, long expectedDelay, long responseLifetime, string asyncResourceUrl, string *stylesheet_ref=0);

	/**
	 * @brief Write the DAP4 AsyncPending response.
	 * Write the DAP4 AsyncPending in XML form.
	 * @param xml Print to this XMLWriter instance
	 */
	void writeD4AsyncPending(XMLWriter &xml, string *stylesheet_ref=0);


	/**
	 * @brief Write the DAP4 AsyncResponseGone response.
	 * Write the DAP4 AsyncRequired in XML form.
	 * @param xml Print to this XMLWriter instance
	 */
	void writeD4AsyncResponseGone(XMLWriter &xml, string *stylesheet_ref=0);

	/**
	 * @brief Write the DAP4 ResponseRejected response.
	 * Write the DAP4 AsyncRequired in XML form.
	 * @param xml Print to this XMLWriter instance
	 */
	void writeD4AsyncResponseRejected(XMLWriter &xml, RejectReasonCode code, string description, string *stylesheet_ref=0);
	string getRejectReasonCodeString(RejectReasonCode code);

	/**
	 * @brief Write the DAP2 AsyncRequired response .
	 * Write the DAP2 AsyncRequired in XML form.
	 * @param xml Print to this XMLWriter instance
	 */
	void writeD2AsyncRequired(XMLWriter &xml, long expectedDelay, long responseLifetime);
	void writeD2AsyncAccepted(XMLWriter &xml, long expectedDelay, long responseLifetime, string asyncResourceUrl);
	void writeD2AsyncPending(XMLWriter &xml);
	void writeD2AsyncResponseGone(XMLWriter &xml);
	void writeD2AsyncResponseRejected(XMLWriter &xml, RejectReasonCode code, string description);


};

} /* namespace libdap */
#endif /* D4ASYNCUTIL_H_ */
