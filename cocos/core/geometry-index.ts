import { legacyCC } from './global-exports';
import * as geometry from './geometry';
import '../misc/intersect';

legacyCC.geometry = geometry;

export { geometry };
