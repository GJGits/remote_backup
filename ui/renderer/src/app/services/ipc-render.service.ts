import { Injectable } from '@angular/core';
import { IpcRenderer } from 'electron';
import { Subject } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class IpcRenderService {

  private ipcRenderer: IpcRenderer | undefined;
  private subjectsMap = new Map();

  constructor() {
    if ((<any>window).require) {
      try {
        this.ipcRenderer = (<any>window).require('electron').ipcRenderer;
        this.ipcRenderer.on("file-upload", (event, data) => { this.onFileUpload(event, data) });
      } catch (e) {
        throw e;
      }
    } else {
      console.warn('App not running inside Electron!');
    }
  }

  private onFileUpload(event, data) {
    if (!this.subjectsMap.has("file-upload")) {
      this.subjectsMap["file-upload"] = new Subject<any>();
    }
    this.subjectsMap["file-upload"].next(data);
  }

  public getFileUploadSubject() {
    return this.subjectsMap["file-upload"];
  }

}
