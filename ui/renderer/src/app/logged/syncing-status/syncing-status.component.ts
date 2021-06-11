import { Component, OnInit } from '@angular/core';
import { IpcRenderService } from 'src/app/services/ipc-render.service';
import { Status, Transfer } from '../models';

@Component({
  selector: 'app-syncing-status',
  templateUrl: './syncing-status.component.html',
  styleUrls: ['./syncing-status.component.css']
})
export class SyncingStatusComponent implements OnInit {

  status: Status = Status.Synced;
  transfers: Transfer[] = [];

  constructor(private ipcService: IpcRenderService) {
    this.ipcService.on('transfer')?.subscribe((transfer) => this.onTrasnfer(transfer));
  }

  ngOnInit(): void {
  }

  private onTrasnfer(transfer: Transfer) {
    if (transfer.status === Status.Synced) {
      let transferIndex = this.transfers.findIndex((el) => { el.path === transfer.path });
      if (transferIndex)
        this.transfers.splice(transferIndex, 1);
      if (this.transfers.length === 0) 
        this.status = Status.Synced;
    } else {
      this.transfers.push(transfer);
      this.status = Status.Syncing;
    }
  }

  public get Status() {
    return Status;
  }



}
