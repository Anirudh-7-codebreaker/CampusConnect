#include "connection_request.h"
#include <iomanip>
#include <ctime>
#include <algorithm>

std::string requestStatusToString(RequestStatus status) {
    switch (status) {
        case RequestStatus::PENDING:  return "PENDING";
        case RequestStatus::ACCEPTED: return "ACCEPTED";
        case RequestStatus::REJECTED: return "REJECTED";
        default:                      return "UNKNOWN";
    }
}

RequestStatus stringToRequestStatus(const std::string& str) {
    std::string upper = str;
    for (char &c : upper) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    if (upper == "ACCEPTED") return RequestStatus::ACCEPTED;
    if (upper == "REJECTED") return RequestStatus::REJECTED;
    return RequestStatus::PENDING;
}

// Default constructor
ConnectionRequest::ConnectionRequest()
    : requestId(0), senderId(0), receiverId(0), status(RequestStatus::PENDING), timestamp("") {}

// Parameterized constructor
ConnectionRequest::ConnectionRequest(int requestId, int senderId, int receiverId,
                                     RequestStatus status, const std::string& timestamp)
    : requestId(requestId), senderId(senderId), receiverId(receiverId), status(status), timestamp(timestamp) {
    if (this->timestamp.empty()) {
        std::time_t now = std::time(nullptr);
        char buf[64];
        if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&now))) {
            this->timestamp = buf;
        } else {
            this->timestamp = "2026-10-16 10:00";
        }
    }
}

int ConnectionRequest::getRequestId() const {
    return requestId;
}

int ConnectionRequest::getSenderId() const {
    return senderId;
}

int ConnectionRequest::getReceiverId() const {
    return receiverId;
}

RequestStatus ConnectionRequest::getStatus() const {
    return status;
}

std::string ConnectionRequest::getStatusString() const {
    return requestStatusToString(status);
}

std::string ConnectionRequest::getTimestamp() const {
    return timestamp;
}

/**
 * @brief Validates and executes state transition for connection request.
 *
 * State Machine:
 * - PENDING -> ACCEPTED (valid)
 * - PENDING -> REJECTED (valid)
 * - ACCEPTED -> any (invalid: terminal state)
 * - REJECTED -> any (invalid: terminal state)
 */
bool ConnectionRequest::updateStatus(RequestStatus newStatus) {
    if (status != RequestStatus::PENDING) {
        return false; // Terminal states cannot be changed
    }
    if (newStatus == RequestStatus::PENDING) {
        return false; // Cannot transition to self
    }
    status = newStatus;
    return true;
}

void ConnectionRequest::display() const {
    std::string tag = "[PENDING]";
    if (status == RequestStatus::ACCEPTED) tag = "[ACCEPTED]";
    else if (status == RequestStatus::REJECTED) tag = "[REJECTED]";

    std::cout << "  Req #" << std::left << std::setw(4) << requestId
              << " | From: Student #" << std::setw(4) << senderId
              << " -> To: Student #" << std::setw(4) << receiverId
              << " | Status: " << std::setw(10) << tag
              << " | Time: " << timestamp << "\n";
}

bool ConnectionRequest::operator==(const ConnectionRequest& other) const {
    return requestId == other.requestId;
}

std::ostream& operator<<(std::ostream& os, const ConnectionRequest& req) {
    os << "Request #" << req.requestId << " (" << req.senderId << " -> "
       << req.receiverId << ", " << req.getStatusString() << ")";
    return os;
}
