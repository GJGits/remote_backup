import { Observable } from "rxjs";

export abstract class IpcRenderService {

  abstract on(channel: string): Observable<any> | undefined;
  abstract send(channel: string, data?: any): void;
  abstract invoke(channel: string, data?: any): Observable<any> | undefined;

}
