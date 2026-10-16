import React, { useState } from 'react';
import { Student, ConnectionRequest } from '../types';
import { X, Check, UserX, Clock, ArrowUpRight, ArrowDownLeft } from 'lucide-react';

interface ConnectionRequestsModalProps {
  isOpen: boolean;
  onClose: () => void;
  requests: ConnectionRequest[];
  students: Student[];
  currentUserId: number;
  onAcceptRequest: (requestId: string) => void;
  onRejectRequest: (requestId: string) => void;
}

export const ConnectionRequestsModal: React.FC<ConnectionRequestsModalProps> = ({
  isOpen,
  onClose,
  requests,
  students,
  currentUserId,
  onAcceptRequest,
  onRejectRequest,
}) => {
  const [activeTab, setActiveTab] = useState<'incoming' | 'outgoing'>('incoming');

  if (!isOpen) return null;

  const incoming = requests.filter((r) => r.recipientId === currentUserId);
  const outgoing = requests.filter((r) => r.senderId === currentUserId);

  const getStudent = (id: number) => students.find((s) => s.id === id);

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-slate-900/50 p-4 backdrop-blur-xs">
      <div className="w-full max-w-lg rounded-2xl bg-white shadow-xl ring-1 ring-slate-900/10 overflow-hidden">
        {/* Header */}
        <div className="flex items-center justify-between border-b border-slate-200 px-6 py-4">
          <div>
            <h2 className="text-lg font-bold text-slate-900">Connection Requests</h2>
            <p className="text-xs text-slate-500">
              Manage your networking requests and peer invitations
            </p>
          </div>
          <button
            onClick={onClose}
            className="rounded-lg p-1.5 text-slate-400 hover:bg-slate-100 hover:text-slate-600 transition"
          >
            <X className="h-5 w-5" />
          </button>
        </div>

        {/* Tabs */}
        <div className="flex border-b border-slate-200 px-6">
          <button
            onClick={() => setActiveTab('incoming')}
            className={`flex items-center gap-2 border-b-2 py-3 text-xs font-semibold transition ${
              activeTab === 'incoming'
                ? 'border-indigo-600 text-indigo-600'
                : 'border-transparent text-slate-500 hover:text-slate-700'
            }`}
          >
            <ArrowDownLeft className="h-4 w-4" />
            Received ({incoming.filter((r) => r.status === 'Pending').length} pending)
          </button>
          <button
            onClick={() => setActiveTab('outgoing')}
            className={`ml-6 flex items-center gap-2 border-b-2 py-3 text-xs font-semibold transition ${
              activeTab === 'outgoing'
                ? 'border-indigo-600 text-indigo-600'
                : 'border-transparent text-slate-500 hover:text-slate-700'
            }`}
          >
            <ArrowUpRight className="h-4 w-4" />
            Sent ({outgoing.length})
          </button>
        </div>

        {/* Content */}
        <div className="max-h-96 overflow-y-auto p-6 space-y-3">
          {activeTab === 'incoming' ? (
            incoming.length === 0 ? (
              <div className="py-10 text-center text-sm text-slate-400">
                No incoming connection requests.
              </div>
            ) : (
              incoming.map((req) => {
                const sender = getStudent(req.senderId);
                return (
                  <div
                    key={req.id}
                    className="flex items-center justify-between rounded-xl border border-slate-200 p-3.5 hover:border-slate-300 transition"
                  >
                    <div>
                      <div className="font-semibold text-slate-800 text-sm">
                        {sender ? sender.name : `Student #${req.senderId}`}
                      </div>
                      <div className="text-xs text-slate-500">
                        {sender?.department} • Year {sender?.year}
                      </div>
                      <div className="mt-1 flex flex-wrap gap-1">
                        {sender?.interests.slice(0, 3).map((item) => (
                          <span
                            key={item}
                            className="rounded bg-slate-100 px-1.5 py-0.5 text-[10px] text-slate-600"
                          >
                            {item}
                          </span>
                        ))}
                      </div>
                    </div>

                    <div className="flex items-center gap-2">
                      {req.status === 'Pending' ? (
                        <>
                          <button
                            onClick={() => onAcceptRequest(req.id)}
                            className="flex items-center gap-1 rounded-lg bg-emerald-600 px-3 py-1.5 text-xs font-semibold text-white hover:bg-emerald-700 transition"
                          >
                            <Check className="h-3.5 w-3.5" />
                            Accept
                          </button>
                          <button
                            onClick={() => onRejectRequest(req.id)}
                            className="flex items-center gap-1 rounded-lg border border-slate-200 px-3 py-1.5 text-xs font-semibold text-slate-600 hover:bg-slate-100 transition"
                          >
                            <UserX className="h-3.5 w-3.5" />
                            Decline
                          </button>
                        </>
                      ) : (
                        <span
                          className={`rounded-full px-2.5 py-1 text-xs font-medium ${
                            req.status === 'Accepted'
                              ? 'bg-emerald-100 text-emerald-700'
                              : 'bg-rose-100 text-rose-700'
                          }`}
                        >
                          {req.status}
                        </span>
                      )}
                    </div>
                  </div>
                );
              })
            )
          ) : outgoing.length === 0 ? (
            <div className="py-10 text-center text-sm text-slate-400">
              You haven&apos;t sent any requests yet.
            </div>
          ) : (
            outgoing.map((req) => {
              const recipient = getStudent(req.recipientId);
              return (
                <div
                  key={req.id}
                  className="flex items-center justify-between rounded-xl border border-slate-200 p-3.5"
                >
                  <div>
                    <div className="font-semibold text-slate-800 text-sm">
                      {recipient ? recipient.name : `Student #${req.recipientId}`}
                    </div>
                    <div className="text-xs text-slate-500">
                      {recipient?.department} • Year {recipient?.year}
                    </div>
                  </div>

                  <div>
                    <span
                      className={`inline-flex items-center gap-1.5 rounded-full px-2.5 py-1 text-xs font-semibold ${
                        req.status === 'Pending'
                          ? 'bg-amber-100 text-amber-800'
                          : req.status === 'Accepted'
                          ? 'bg-emerald-100 text-emerald-800'
                          : 'bg-rose-100 text-rose-800'
                      }`}
                    >
                      {req.status === 'Pending' && <Clock className="h-3 w-3" />}
                      {req.status === 'Accepted' && <Check className="h-3 w-3" />}
                      {req.status}
                    </span>
                  </div>
                </div>
              );
            })
          )}
        </div>
      </div>
    </div>
  );
};
