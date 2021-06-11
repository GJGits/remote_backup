import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MainComponent } from './main/main.component';
import { DummyComponentsModule } from '../dummy-components/dummy-components.module';
import { SyncingStatusComponent } from './syncing-status/syncing-status.component';
import { LoggedRoutingModule } from './logged-routing.module';
import { InfosComponent } from './infos/infos.component';
import { ConnectionLostComponent } from './connection-lost/connection-lost.component';



@NgModule({
  declarations: [
    MainComponent,
    SyncingStatusComponent,
    InfosComponent,
    ConnectionLostComponent
  ],
  imports: [
    CommonModule,
    DummyComponentsModule,
    LoggedRoutingModule,
  ]
})
export class LoggedModule { }
