import { RealCurve } from '../core/curves/curve';
import { ccclass, serializable } from '../core/data/decorators';
import { CLASS_NAME_PREFIX_ANIM } from './define';

@ccclass(`${CLASS_NAME_PREFIX_ANIM}AuxiliaryCurveEntry`)
export class AuxiliaryCurveEntry {
    @serializable
    public name = '';

    @serializable
    public curve = new RealCurve();
}
