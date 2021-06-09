import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { IpcRenderService } from './ipc-render.service';
import { environment } from 'src/environments/environment';
import { ProdIpcService } from './prod-ipc.service';
import { MockIpcService } from './mock-ipc.service';

export function ipcFactory(): IpcRenderService {
  switch(environment.production){
    case true: {return new ProdIpcService()}
    default: {return new MockIpcService()}
  }
}

@NgModule({
  declarations: [],
  imports: [
    CommonModule
  ]
})
export class ServicesModule { 

}
