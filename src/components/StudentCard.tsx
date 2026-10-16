import React from 'react';
import { Student, ConnectionRequest } from '../types';
import { UserPlus, Check, Clock, UserX, User, Sparkles } from 'lucide-react';

interface StudentCardProps {
  student: Student;
  isSelf: boolean;
  outgoingRequest?: ConnectionRequest;
  incomingRequest?: ConnectionRequest;
  onSendRequest: (recipientId: number) => void;
  onAcceptRequest: (requestId: string) => void;
  onRejectRequest: (requestId: string) => void;
  onSelectInterest: (interest: string) => void;
  selectedInterests: string[];
}

export const StudentCard: React.FC<StudentCardProps> = ({
  student,
  isSelf,
  outgoingRequest,
  incomingRequest,
  onSendRequest,
  onAcceptRequest,
  onRejectRequest,
  onSelectInterest,
  selectedInterests,
}) => {
  // Generate consistent pleasant background color based on student ID
  const avatarColors = [
    'bg-indigo-600',
    'bg-violet-600',
    'bg-sky-600',
    'bg-emerald-600',
    'bg-amber-600',
    'bg-rose-600',
  ];
  const colorIndex = student.id % avatarColors.length;
  const avatarBg = avatarColors[colorIndex];

  // Check connection status
  const isConnected =
    outgoingRequest?.status === 'Accepted' || incomingRequest?.status === 'Accepted';
  const isPendingOutgoing = outgoingRequest?.status === 'Pending';
  const isPendingIncoming = incomingRequest?.status === 'Pending';
  const isRejected = outgoingRequest?.status === 'Rejected';

  return (
    <div className="flex flex-col justify-between rounded-2xl border border-slate-200 bg-white p-5 shadow-xs transition hover:shadow-md hover:border-slate-300">
      <div>
        {/* Top Info */}
        <div className="flex items-start justify-between gap-3">
          <div className="flex items-center gap-3">
            <div
              className={`flex h-12 w-12 items-center justify-center rounded-xl text-lg font-bold text-white shadow-xs ${avatarBg}`}
            >
              {student.name.charAt(0)}
            </div>
            <div>
              <div className="flex items-center gap-2">
                <h3 className="font-semibold text-slate-900 leading-tight">
                  {student.name}
                </h3>
                {isSelf && (
                  <span className="rounded-full bg-indigo-100 px-2 py-0.5 text-[10px] font-semibold text-indigo-700">
                    You
                  </span>
                )}
              </div>
              <div className="text-xs text-slate-500 flex items-center gap-1.5 mt-0.5">
                <span className="font-medium text-slate-700">{student.department}</span>
                <span>•</span>
                <span>Year {student.year}</span>
                <span>•</span>
                <span className="font-mono text-[11px] text-slate-400">#{student.id}</span>
              </div>
            </div>
          </div>
        </div>

        {/* Bio if present */}
        {student.bio && (
          <p className="mt-3 text-xs text-slate-600 line-clamp-2">
            {student.bio}
          </p>
        )}

        {/* Interests */}
        <div className="mt-4">
          <span className="text-[11px] font-semibold uppercase tracking-wider text-slate-400">
            Interests
          </span>
          <div className="mt-1.5 flex flex-wrap gap-1.5">
            {student.interests.map((interest) => {
              const isMatch = selectedInterests.some(
                (sel) => sel.toLowerCase() === interest.toLowerCase()
              );
              return (
                <button
                  key={interest}
                  type="button"
                  onClick={() => onSelectInterest(interest)}
                  className={`inline-flex items-center gap-1 rounded-lg px-2.5 py-1 text-xs font-medium transition cursor-pointer ${
                    isMatch
                      ? 'bg-indigo-600 text-white shadow-xs'
                      : 'bg-slate-100 text-slate-700 hover:bg-slate-200'
                  }`}
                  title={`Filter by ${interest}`}
                >
                  {isMatch && <Sparkles className="h-3 w-3" />}
                  {interest}
                </button>
              );
            })}
          </div>
        </div>
      </div>

      {/* Footer / Connection Actions */}
      <div className="mt-5 border-t border-slate-100 pt-3">
        {isSelf ? (
          <div className="flex items-center justify-center gap-1.5 py-1 text-xs font-medium text-slate-400">
            <User className="h-3.5 w-3.5" />
            Your Profile
          </div>
        ) : isConnected ? (
          <div className="flex items-center justify-between rounded-xl bg-emerald-50 px-3 py-2 text-xs font-semibold text-emerald-700 border border-emerald-100">
            <span className="flex items-center gap-1.5">
              <Check className="h-4 w-4 text-emerald-600" />
              Connected
            </span>
            <span className="text-[11px] font-normal text-emerald-600">Peers</span>
          </div>
        ) : isPendingIncoming ? (
          <div className="space-y-1.5">
            <p className="text-[11px] font-medium text-amber-700 bg-amber-50 rounded-lg px-2 py-1 text-center border border-amber-100">
              Sent you a connection request
            </p>
            <div className="flex gap-2">
              <button
                onClick={() => incomingRequest && onAcceptRequest(incomingRequest.id)}
                className="flex-1 flex items-center justify-center gap-1 rounded-lg bg-emerald-600 py-1.5 text-xs font-semibold text-white shadow-xs hover:bg-emerald-700 transition"
              >
                <Check className="h-3.5 w-3.5" />
                Accept
              </button>
              <button
                onClick={() => incomingRequest && onRejectRequest(incomingRequest.id)}
                className="flex-1 flex items-center justify-center gap-1 rounded-lg border border-slate-200 bg-white py-1.5 text-xs font-semibold text-slate-700 hover:bg-slate-50 transition"
              >
                <UserX className="h-3.5 w-3.5" />
                Decline
              </button>
            </div>
          </div>
        ) : isPendingOutgoing ? (
          <div className="flex items-center justify-center gap-1.5 rounded-xl border border-amber-200 bg-amber-50/70 py-2 text-xs font-semibold text-amber-700">
            <Clock className="h-3.5 w-3.5 animate-pulse" />
            Request Sent (Pending)
          </div>
        ) : isRejected ? (
          <div className="flex items-center justify-between py-1">
            <span className="text-xs text-slate-400">Request Declined</span>
            <button
              onClick={() => onSendRequest(student.id)}
              className="rounded-lg border border-slate-200 px-2.5 py-1 text-xs font-medium text-slate-600 hover:bg-slate-50 transition"
            >
              Retry
            </button>
          </div>
        ) : (
          <button
            onClick={() => onSendRequest(student.id)}
            className="w-full flex items-center justify-center gap-1.5 rounded-xl bg-indigo-600 py-2 text-xs font-semibold text-white shadow-xs hover:bg-indigo-700 transition active:scale-[0.99]"
          >
            <UserPlus className="h-3.5 w-3.5" />
            Connect
          </button>
        )}
      </div>
    </div>
  );
};
