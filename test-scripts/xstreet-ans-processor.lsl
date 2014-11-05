// Xstreet SL ANS Processing Module
// v1.1.4
//
// This script is used for post-processing Xstreet SL transactions. It receives ANS notifications, optionally verifies them,
// and then makes the data available for any custom actions.
//
// 1.0.0 - 2006-11-13 - Initial release
// 1.1.0 - 2006-11-18 - Updated to include optional verification and extra data fields provided by verification enhancements
// 1.1.1 - 2007-06-12 - Fixed incorrect value "VerifyKey" when data not present for a field
// 1.1.2 - 2007-06-23 - Commented debugging error messages
// 1.1.3 - 2008-10-03 - Changed name to Xstreet SL, updated URL
// 1.1.4 - 2009-04-21 - Updated URL to use HTTPS


// Set to TRUE to verify notifications or FALSE to use the data as provided
integer VerifyNotifications = FALSE;

// Set to the Xstreet SL ANS verification URL
string VerifyUrl = "https://www.xstreetsl.com/ans.php?VerifyKey=";

integer LM_RX_XMLRPC = 4;               // xmlrpc receive
integer LM_TX_XMLRPC = 5;               // xmlrpc send

// ParseAnsNotification() method
// Parses a pipe-delimited notification string into a strided list
list ParseAnsNotification(string notificationString) {
    integer i;
    list notificationList = llParseStringKeepNulls(notificationString, "[\"|\",\"=\"], []");
    return notificationList;
}


// ProcessNotification() method
// Sample
ProcessAnsNotification(list notificationList, integer verified) {
    // Populate variables
    string verifyKey = GetNotificationValue(notificationList, "VerifyKey");
    string type = GetNotificationValue(notificationList, "Type");
    string currency = GetNotificationValue(notificationList, "Currency");
    string paymentGross = GetNotificationValue(notificationList, "PaymentGross");
    string paymentFee = GetNotificationValue(notificationList, "PaymentFee");
    string payerName = GetNotificationValue(notificationList, "PayerName");
    string payerKey = GetNotificationValue(notificationList, "PayerKey");
    string receiverName = GetNotificationValue(notificationList, "ReceiverName");
    string receiverKey = GetNotificationValue(notificationList, "ReceiverKey");
    string merchantName = GetNotificationValue(notificationList, "MerchantName");
    string merchantKey = GetNotificationValue(notificationList, "MerchantKey");
    string transactionID = GetNotificationValue(notificationList, "TransactionID");
    string itemID = GetNotificationValue(notificationList, "ItemID");
    string itemName = GetNotificationValue(notificationList, "ItemName");
    string region = GetNotificationValue(notificationList, "Region");
    string location = GetNotificationValue(notificationList, "Location");
    
//    if (!verified && VerifyNotifications) {
        // Send HTTP request for verification
        llHTTPRequest(VerifyUrl + llEscapeURL(verifyKey), HTTP_METHOD, "GET", "");
//    } else {
        llOwnerSay("VerifyKey = " + verifyKey);
        llOwnerSay("Type = " + type);
        llOwnerSay("Currency = " + currency);
        llOwnerSay("PaymentGross = " + paymentGross);
        llOwnerSay("PaymentFee = " + paymentFee);
        llOwnerSay("PayerName = " + payerName);
        llOwnerSay("PayerKey = " + payerKey);
        llOwnerSay("ReceiverName = " + receiverName);
        llOwnerSay("ReceiverKey = " + receiverKey);
        llOwnerSay("MerchantName = " + merchantName);
        llOwnerSay("MerchantKey = " + merchantKey);
        llOwnerSay("TransactionID = " + transactionID);
        llOwnerSay("ItemID = " + itemID);
        llOwnerSay("ItemName = " + itemName);
        llOwnerSay("Region = " + region);
        llOwnerSay("Location = " + location);
        
        // TODO:
        // Check that the PaymentGross is correct
        // Process the payment
//    }
}





//////////////////////////////////////////
// Nothing below should need to be changed

// GetNotificationValue() method
// Gets the value associated with the given ANS variable within the given notification list
string GetNotificationValue(list notificationList, string variableName) {
    integer variableIndex = llListFindList(notificationList, [variableName]);
    if (variableIndex >= 0 && llGetListLength(notificationList) > variableIndex + 1) {
        return(llList2String(notificationList, variableIndex + 1));
    }
    return "";
}

// ParseAnsNotification() method
// Parses a pipe-delimited notification string into a strided list
list ParseAnsNotification(string notificationString) {
    integer i;
    list notificationList = llParseStringKeepNulls(notificationString, "|","=", "");
    return notificationList;
}

default {
    state_entry() {
    }

    link_message(integer sender, integer idat, string sdat, key kdat) {
        if (idat == LM_RX_XMLRPC) {
            if (llSubStringIndex(sdat, "VerifyKey") == 0) {
                llMessageLinked(llGetLinkNumber(), LM_TX_XMLRPC, "ok", NULL_KEY);
                list myNotification = ParseAnsNotification(sdat);
                ProcessAnsNotification(myNotification, FALSE);
            }
        }
    }
    
    http_response(key id, integer status, list metadata, string body) {
        // Process verification reply
        if (llSubStringIndex(body, "SUCCESS") == 0 && llStringLength(body) > 8) {
            // Verification succeeded
            list myNotification = ParseAnsNotification(llGetSubString(body, 8, -1));
            ProcessAnsNotification(myNotification, TRUE);
        } else if (llSubStringIndex(body, "FAILURE") == 0) {
            // Verification failed
//            llOwnerSay("Verification failed");
        } else {
            // Some error occurred
//            llOwnerSay("Some error occurred: " + body);
        }
    }
}
