import { Component, Input, OnInit } from '@angular/core';
import { UsageInfo } from 'src/app/logged/models';

@Component({
  selector: 'app-footer-infos',
  templateUrl: './footer-infos.component.html',
  styleUrls: ['./footer-infos.component.css']
})
export class FooterInfosComponent implements OnInit {

  @Input() usageInfo: UsageInfo | null = {current_byte_used: 0, total_byte_allowed: 0};

  constructor() { }

  ngOnInit(): void {
  }

}
