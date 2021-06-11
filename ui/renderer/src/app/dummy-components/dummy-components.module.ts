import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ToolBarComponent } from './tool-bar/tool-bar.component';
import { FooterInfosComponent } from './footer-infos/footer-infos.component';
import { UsagePipe } from './usage.pipe';



@NgModule({
  declarations: [
    ToolBarComponent,
    FooterInfosComponent,
    UsagePipe
  ],
  imports: [
    CommonModule
  ],
  exports: [ToolBarComponent,FooterInfosComponent]
})
export class DummyComponentsModule { }
