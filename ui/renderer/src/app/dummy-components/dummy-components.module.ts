import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ToolBarComponent } from './tool-bar/tool-bar.component';
import { FooterInfosComponent } from './footer-infos/footer-infos.component';



@NgModule({
  declarations: [
    ToolBarComponent,
    FooterInfosComponent
  ],
  imports: [
    CommonModule
  ],
  exports: [ToolBarComponent,FooterInfosComponent]
})
export class DummyComponentsModule { }
