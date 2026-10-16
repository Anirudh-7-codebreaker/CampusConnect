export interface Student {
  id: number;
  name: string;
  department: string;
  year: number;
  interests: string[];
  avatarUrl?: string;
  bio?: string;
}

export type RequestStatus = 'Pending' | 'Accepted' | 'Rejected';

export interface ConnectionRequest {
  id: string;
  senderId: number;
  recipientId: number;
  status: RequestStatus;
  createdAt: string;
}
