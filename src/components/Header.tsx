import React from 'react';
import { Student } from '../types';
import {
  GraduationCap,
  Terminal,
  Bell,
  UserPlus,
  UserCheck,
} from 'lucide-react';

interface HeaderProps {
  students: Student[];
  currentUserId: number;
  onSelectCurrentUser: (id: number) => void;
  pendingRequestsCount: number;
  onOpenRequests: () => void;
  onOpenAddStudent: () => void;
  onOpenTerminal: () => void;
}

export const Header: React.FC<HeaderProps> = ({
  students,
  currentUserId,
  onSelectCurrentUser,
  pendingRequestsCount,
  onOpenRequests,
  onOpenAddStudent,
  onOpenTerminal,
}) => {
  const currentUser = students.find((s) => s.id === currentUserId) || students[0];

  return (
    <header className="sticky top-0 z-30 border-b border-slate-200 bg-white/95 backdrop-blur">
      <div className="mx-auto flex max-w-7xl items-center justify-between px-4 py-3 sm:px-6">
        {/* Brand */}
        <div className="flex items-center gap-3">
          <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-indigo-600 text-white shadow-md shadow-indigo-200">
            <GraduationCap className="h-6 w-6" />
          </div>
          <div>
            <div className="flex items-center gap-2">
              <span className="text-xl font-bold tracking-tight text-slate-900">
                Campus Connect
              </span>
              <span className="rounded bg-indigo-50 px-2 py-0.5 text-xs font-semibold text-indigo-700">
                v1.0
              </span>
            </div>
            <p className="hidden text-xs text-slate-500 sm:block">
              Peer discovery & collaboration for university students
            </p>
          </div>
        </div>

        {/* Actions & User Selector */}
        <div className="flex items-center gap-2 sm:gap-3">
          {/* Terminal / C++ Mode */}
          <button
            onClick={onOpenTerminal}
            className="flex items-center gap-1.5 rounded-lg border border-slate-300 bg-slate-100 px-3 py-1.5 text-xs font-medium text-slate-700 hover:bg-slate-200 transition"
            title="Open C++ Console Terminal View"
          >
            <Terminal className="h-4 w-4 text-emerald-600" />
            <span className="hidden md:inline">C++ CLI Mode</span>
          </button>

          {/* Add Student */}
          <button
            onClick={onOpenAddStudent}
            className="flex items-center gap-1.5 rounded-lg border border-slate-200 bg-white px-3 py-1.5 text-xs font-medium text-slate-700 hover:bg-slate-50 transition"
          >
            <UserPlus className="h-4 w-4 text-indigo-600" />
            <span className="hidden sm:inline">Add Student</span>
          </button>

          {/* Requests Inbox */}
          <button
            onClick={onOpenRequests}
            className="relative flex items-center gap-1.5 rounded-lg border border-slate-200 bg-white px-3 py-1.5 text-xs font-medium text-slate-700 hover:bg-slate-50 transition"
          >
            <Bell className="h-4 w-4 text-slate-600" />
            <span className="hidden sm:inline">Requests</span>
            {pendingRequestsCount > 0 && (
              <span className="flex h-5 min-w-5 items-center justify-center rounded-full bg-rose-500 px-1 text-[11px] font-bold text-white">
                {pendingRequestsCount}
              </span>
            )}
          </button>

          {/* Current User Switcher */}
          <div className="flex items-center gap-2 rounded-xl border border-indigo-100 bg-indigo-50/70 py-1 pl-2.5 pr-2">
            <div className="hidden text-right text-xs lg:block">
              <div className="font-semibold text-slate-800 leading-tight">
                {currentUser?.name}
              </div>
              <div className="text-[11px] text-indigo-600">
                {currentUser?.department} • Year {currentUser?.year} (ID: {currentUser?.id})
              </div>
            </div>
            <label htmlFor="user-switch" className="sr-only">Switch active student</label>
            <select
              id="user-switch"
              value={currentUserId}
              onChange={(e) => onSelectCurrentUser(Number(e.target.value))}
              className="cursor-pointer rounded-lg border border-indigo-200 bg-white py-1 px-2 text-xs font-medium text-slate-800 shadow-xs focus:border-indigo-500 focus:outline-none focus:ring-1 focus:ring-indigo-500"
              title="Switch active student perspective"
            >
              {students.map((s) => (
                <option key={s.id} value={s.id}>
                  {s.name} ({s.id} - {s.department})
                </option>
              ))}
            </select>
          </div>
        </div>
      </div>
    </header>
  );
};
