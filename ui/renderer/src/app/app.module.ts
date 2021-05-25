import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { AuthRoutingModule } from './auth/auth-routing.module';
import { DummyComponentsModule } from './dummy-components/dummy-components.module';
import { FooterInfosComponent } from './dummy-components/footer-infos/footer-infos.component';
import { ToolBarComponent } from './dummy-components/tool-bar/tool-bar.component';
import { SyncingStatusComponent } from './syncing-status/syncing-status.component';

@NgModule({
  declarations: [
    AppComponent,
    SyncingStatusComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    DummyComponentsModule,
    AuthRoutingModule
  ],
  providers: [],
  exports: [SyncingStatusComponent],
  bootstrap: [AppComponent]
})
export class AppModule { }
