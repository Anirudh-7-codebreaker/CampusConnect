#ifndef CONNECTION_REQUEST_H
#define CONNECTION_REQUEST_H

#include <string>
#include <iostream>

/**
 * @brief Represents the lifecycle status of a skill-exchange connection request.
 */
enum class RequestStatus {
    PENDING,
    ACCEPTED,
    REJECTED
};

std::string requestStatusToString(RequestStatus status);
RequestStatus stringToRequestStatus(const std::string& str);

/**
 * @brief Represents a peer-to-peer connection request between two students.
 *
 * Enforces business logic:
 * - Senders cannot send requests to themselves.
 * - Allowed status transitions: only PENDING -> ACCEPTED or PENDING -> REJECTED.
 */
class ConnectionRequest {
private:
    int requestId;
    int senderId;
    int receiverId;
    RequestStatus status;
    std::string timestamp;

public:
    // Constructors
    ConnectionRequest();
    ConnectionRequest(int requestId, int senderId, int receiverId,
                      RequestStatus status = RequestStatus::PENDING,
                      const std::string& timestamp = "");

    // Getters
    int getRequestId() const;
    int getSenderId() const;
    int getReceiverId() const;
    RequestStatus getStatus() const;
    std::string getStatusString() const;
    std::string getTimestamp() const;

    // Status transition validation (returns false if invalid state transition)
    bool updateStatus(RequestStatus newStatus);

    // Display request summary
    void display() const;

    // Operator Overloading for checking equality of requests
    bool operator==(const ConnectionRequest& other) const;

    // Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const ConnectionRequest& req);
};

#endif // CONNECTION_REQUEST_H
