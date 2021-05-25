import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MainComponent } from './main/main.component';
import { ToolBarComponent } from '../dummy-components/tool-bar/tool-bar.component';
import { FooterInfosComponent } from '../dummy-components/footer-infos/footer-infos.component';
import { SyncingStatusComponent } from '../syncing-status/syncing-status.component';



@NgModule({
  declarations: [
    MainComponent
  ],
  imports: [
    CommonModule,
    ToolBarComponent,
    FooterInfosComponent,
    SyncingStatusComponent
  ]
})
export class LoggedModule { }
