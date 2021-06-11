import { Component, OnInit } from '@angular/core';
import { timer } from 'rxjs';

@Component({
  selector: 'app-connection-lost',
  templateUrl: './connection-lost.component.html',
  styleUrls: ['./connection-lost.component.css']
})
export class ConnectionLostComponent implements OnInit {

  time = 30;

  constructor() { }

  ngOnInit(): void {
    timer(1000,1000).subscribe((value) => {
      if (this.time > 0)
        this.time--;
      else
        this.time = 30;
    });
  }

}
