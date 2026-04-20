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

/** @brief Enumerates standardized async rejection reasons. */
enum RejectReasonCode { TIME, UNAVAILABLE, PRIVILEGES, OTHER };

/** @brief Utility for writing DAP2/DAP4 asynchronous status responses. */
class D4AsyncUtil {
private:
#if 0
    // Not used
	string *d_stylesheet_ref;
#endif

public:
    D4AsyncUtil();
    virtual ~D4AsyncUtil();

    /// XML attribute name used to inject a stylesheet reference.
    const static string STYLESHEET_REFERENCE_KEY;

    /**
     * @brief Write the DAP4 AsyncRequired response.
     * Print the AsyncRequired in XML form.
     * @param xml Print to this XMLWriter instance
     * @param expectedDelay Estimated delay before an accepted response can be polled.
     * @param responseLifetime Maximum lifetime of the eventual asynchronous response.
     * @param stylesheet_ref Optional stylesheet URL to include in the XML.
     */
    void writeD4AsyncRequired(XMLWriter &xml, long expectedDelay, long responseLifetime, string *stylesheet_ref = 0);

    /**
     * @brief Write the DAP4 AsyncAccepted response.
     * Write the AsyncAccepted in XML form.
     * @param xml Print to this XMLWriter instance
     * @param expectedDelay Estimated delay before the result is expected.
     * @param responseLifetime Lifetime of the asynchronous response resource.
     * @param asyncResourceUrl URL clients use to poll for the response.
     * @param stylesheet_ref Optional stylesheet URL to include in the XML.
     */
    void writeD4AsyncAccepted(XMLWriter &xml, long expectedDelay, long responseLifetime, string asyncResourceUrl,
                              string *stylesheet_ref = 0);

    /**
     * @brief Write the DAP4 AsyncPending response.
     * Write the DAP4 AsyncPending in XML form.
     * @param xml Print to this XMLWriter instance
     * @param stylesheet_ref Optional stylesheet URL to include in the XML.
     */
    void writeD4AsyncPending(XMLWriter &xml, string *stylesheet_ref = 0);

    /**
     * @brief Write the DAP4 AsyncResponseGone response.
     * Write the DAP4 AsyncRequired in XML form.
     * @param xml Print to this XMLWriter instance
     * @param stylesheet_ref Optional stylesheet URL to include in the XML.
     */
    void writeD4AsyncResponseGone(XMLWriter &xml, string *stylesheet_ref = 0);

    /**
     * @brief Write the DAP4 ResponseRejected response.
     * Write the DAP4 AsyncRequired in XML form.
     * @param xml Print to this XMLWriter instance
     * @param code Machine-readable reason for rejection.
     * @param description Human-readable rejection description.
     * @param stylesheet_ref Optional stylesheet URL to include in the XML.
     */
    void writeD4AsyncResponseRejected(XMLWriter &xml, RejectReasonCode code, string description,
                                      string *stylesheet_ref = 0);

    /** @brief Convert a rejection code to its protocol string value.
     * @param code Rejection reason code.
     * @return Protocol token for `code`.
     */
    string getRejectReasonCodeString(RejectReasonCode code);

    /**
     * @brief Write the DAP2 AsyncRequired response .
     * Write the DAP2 AsyncRequired in XML form.
     * @param xml Print to this XMLWriter instance
     * @param expectedDelay Estimated delay before an accepted response can be polled.
     * @param responseLifetime Maximum lifetime of the eventual asynchronous response.
     */
    void writeD2AsyncRequired(XMLWriter &xml, long expectedDelay, long responseLifetime);

    /** @brief Write a DAP2 AsyncAccepted XML response.
     * @param xml Destination XML writer.
     * @param expectedDelay Estimated delay before the result is expected.
     * @param responseLifetime Lifetime of the asynchronous response resource.
     * @param asyncResourceUrl URL clients use to poll for the response.
     */
    void writeD2AsyncAccepted(XMLWriter &xml, long expectedDelay, long responseLifetime, string asyncResourceUrl);

    /** @brief Write a DAP2 AsyncPending XML response.
     * @param xml Destination XML writer.
     */
    void writeD2AsyncPending(XMLWriter &xml);

    /** @brief Write a DAP2 AsyncResponseGone XML response.
     * @param xml Destination XML writer.
     */
    void writeD2AsyncResponseGone(XMLWriter &xml);

    /** @brief Write a DAP2 AsyncResponseRejected XML response.
     * @param xml Destination XML writer.
     * @param code Machine-readable reason for rejection.
     * @param description Human-readable rejection description.
     */
    void writeD2AsyncResponseRejected(XMLWriter &xml, RejectReasonCode code, string description);
};

} /* namespace libdap */
#endif /* D4ASYNCUTIL_H_ */
