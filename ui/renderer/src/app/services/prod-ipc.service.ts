import { Injectable } from '@angular/core';
import { IpcRenderer } from 'electron';
import { Observable, Subject } from 'rxjs';
import { from } from 'rxjs';
import { IpcRenderService } from './ipc-render.service';

@Injectable({
  providedIn: 'root'
})
export class ProdIpcService implements IpcRenderService{

  private ipcRenderer: IpcRenderer | undefined;
  private subjectsMap = new Map<string, Subject<any>>();

  constructor() {
    if ((<any>window).require) {
      try {
        this.ipcRenderer = (<any>window).require('electron').ipcRenderer;
        this.ipcRenderer?.on("file-upload", (event, data) => { this.onFileUpload(event, data) });
      } catch (e) {
        throw e;
      }
    } else {
      console.warn('App not running inside Electron!');
    }
  }

  private onFileUpload(event: any, data: any) {
    if (!this.subjectsMap.has("file-upload")) {
      this.subjectsMap.set("file-upload", new Subject<any>());
    }
    this.subjectsMap.get("file-upload")?.next(data);
  }

  public on(channel: string): Observable<any> | undefined {
    if (this.subjectsMap.has(channel) && this.subjectsMap.get(channel))
      return this.subjectsMap.get(channel)?.asObservable();
    return undefined;
  }

  public send(channel: string, data: any) {
    this.ipcRenderer?.send(channel, data);
  }

  public invoke(channel: string, data?: any): Observable<any> | undefined {
    if (this.ipcRenderer)
      return from(this.ipcRenderer?.invoke(channel, data));
    return undefined;
  }

}
