import { Pipe, PipeTransform } from '@angular/core';
import { UsageInfo } from '../logged/models';

@Pipe({
  name: 'usage'
})
export class UsagePipe implements PipeTransform {

  transform(value: UsageInfo | null): string {

    if (value) {

      let usageUnit = 'KB';
      let usage = Math.floor(value.current_byte_used / 1024);
      let total = Math.floor(value.total_byte_allowed / 1073741824);

      if (value.current_byte_used >= 1048576 && value.current_byte_used < 1073741824) {
        usageUnit = 'MB';
        usage = Math.floor(value.current_byte_used / (1024 * 1024));
      }

      if (value.current_byte_used >= 1073741824) {
        usageUnit = 'GB';
        usage = Math.floor(value.current_byte_used / (1024 * 1024 * 1024));
      }

      let percentage = value.current_byte_used / value.total_byte_allowed;
      percentage = Math.round(percentage * 100) / 100;

      return "" + usage + usageUnit + "/" + total + "GB" + " (" + percentage + "%)";

    }

    return "";

  }

}
