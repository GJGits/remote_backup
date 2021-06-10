import { Component, Input, OnInit } from '@angular/core';
import { UserInfo } from 'src/app/auth/dtos';

@Component({
  selector: 'app-tool-bar',
  templateUrl: './tool-bar.component.html',
  styleUrls: ['./tool-bar.component.css']
})
export class ToolBarComponent implements OnInit {

  @Input() userDetails: UserInfo | null = {user: '', device: ''};

  constructor() { }

  ngOnInit(): void {
  }

}
