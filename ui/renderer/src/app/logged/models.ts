export interface UsageInfo {
    current_byte_used: number;
    total_byte_allowed: number;
}

export interface Transfer {
    path: string;
    last_change: number;
    status: Status;
    producer: number;
    nchunks: number;
}

export enum Status {
    Synced,
    Syncing,
    ConnectionLost
  }