import { graphicsAssembler } from '../cocos/2d/assembler/graphics';
import { cclegacy } from '../cocos/core/global-exports';

export { graphicsAssembler }; // use less
export { Graphics } from '../cocos/2d/components/graphics';

cclegacy.UI = cclegacy.UI || {};

cclegacy.UI.graphicsAssembler = graphicsAssembler;
