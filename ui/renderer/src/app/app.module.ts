import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { AuthModule } from './auth/auth.module';
import { DummyComponentsModule } from './dummy-components/dummy-components.module';
import { LoggedModule } from './logged/logged.module';
import { IpcRenderService } from './services/ipc-render.service';
import { ipcFactory } from './services/services.module';

@NgModule({
  declarations: [
    AppComponent,
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    AuthModule,
    LoggedModule,
    DummyComponentsModule,
  ],
  providers: [{
    provide: IpcRenderService,
    useFactory: ipcFactory,
  }],
  bootstrap: [AppComponent]
})
export class AppModule { }
