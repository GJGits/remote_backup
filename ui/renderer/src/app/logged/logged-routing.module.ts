import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule, Routes } from '@angular/router';
import { MainComponent } from './main/main.component';
import { AuthGuard } from '../auth/auth.guard';
import { InfosComponent } from './infos/infos.component';
import { ConnectionLostComponent } from './connection-lost/connection-lost.component';

const routes: Routes = [
  {path: 'app', pathMatch: 'full', redirectTo: 'app/main'},
  {path: 'app/main', component: MainComponent, canActivate: [AuthGuard]},
  {path: 'app/infos', component: InfosComponent, canActivate: [AuthGuard]},
  {path: 'app/connectionlost', component: ConnectionLostComponent, canActivate: [AuthGuard]}
];

@NgModule({
  declarations: [],
  imports: [
    CommonModule,
    RouterModule.forChild(routes)
  ],
  exports: [RouterModule]
})
export class LoggedRoutingModule { }
