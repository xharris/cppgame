// declare module "image" {
  export function newAnimation(opts:AnimationOptions):void

  type AnimationOptions = {
    name:string,
    frames:number[],
    cols:number,
    rows:number,
    speed?:number
  }
// }