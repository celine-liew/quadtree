
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"
#include "math.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 

/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

/* your code here */
std::cout << "making tree now" << endl;
	if (imIn.width() == pow(2,k) && imIn.height() == pow(2,k)){
		std::cout << "going to build tree here! imIn height: " << imIn.height() << "k: " << k << endl;
		std::cout << "going to build tree here! imIn width: " << imIn.width() << "k: " << k << endl;
		buildTree(imIn, k);
	} else {
	pair<int,int> ctrPoint;
	ctrPoint.first = imIn.width()/2;
	ctrPoint.second = imIn.height()/2;
	PNG subIm(ctrPoint.first, ctrPoint.second);
		// grab 2^k * 2 ^k subimage centered
		for (unsigned int i = 0; i < pow(2, k); i++){
			for (unsigned int j = 0; j < pow(2, k); j++){
				HSLAPixel * pixelSub = subIm.getPixel(i, j);
				* pixelSub = *imIn.getPixel(ctrPoint.first - pow(2,k-1) + i, ctrPoint.second - pow(2,k-1) + j);
			}
		}
		std::cout << "going to build tree 2" << endl;
		std::cout << "subimage passed: " << ctrPoint.first << " " << ctrPoint.second << endl;
		buildTree(subIm, k);
	}

}

int toqutree::size() {
	return countSize(root);
/* your code here */
}


toqutree::Node * toqutree::buildTree(PNG & im, int k) {
	// k is big sub-image
	std::cout << "entered BuildTree" << endl;
	stats s(im);
	int width = im.width();
	int height = im.height();
	pair<int,int> ul(0,0);
	pair<int,int> lr(width - 1, height - 1);
	HSLAPixel avgPixel = s.getAvg(ul,lr);
	pair<int,int> optSplitPos;
	// Base cases
	if (k == 0){
		Node* node = new Node(ul, k, avgPixel);
		return node;
	}

	PNG nwChild(width/2, height/2);
	PNG seChild(width/2, height/2);
	PNG swChild(width/2, height/2);
	PNG neChild(width/2, height/2);
	std::cout << "BuildTree before if k = 1" << endl;
	if (k == 1){

		HSLAPixel * pixelNewNW = nwChild.getPixel(0, 0);
		pixelNewNW = im.getPixel(0,0);

		HSLAPixel * pixelNewSe = seChild.getPixel(0, 0);
		pixelNewSe = im.getPixel(1,1);

		HSLAPixel * pixelNewSw = swChild.getPixel(0, 0);
		pixelNewSw = im.getPixel(0,1);
		
		HSLAPixel * pixelNewNe = neChild.getPixel(0, 0);
		pixelNewNe = im.getPixel(1,0);

	}

	
	else {
		std::cout << "build tree - check else" << endl;
		// find centre
		unsigned int ctrUl_x = width/4; //4
		unsigned int ctrUl_y = height/4; // 4
		unsigned int ctrLr_x = width - ctrUl_x - 1; //11
		unsigned int ctrLr_y = height - ctrUl_y - 1; //11

		pair<int,int> ctrul(ctrUl_x, ctrUl_y);
		pair<int,int> ctrlr(ctrLr_x, ctrLr_y);

		double minEnt = 500.0;
		double avgEntropy;
		
		// optSplitPos.first = 0;
		// optSplitPos.second = 0;
		pair<int,int> ulSubRect(0,0);
		pair<int,int> lrSubRect((width/2)-1,(height/2)-1);
		long rectArea = s.rectArea(ulSubRect, lrSubRect);
		// we use this var when we made child nodes: 1-TOPLEFT 2-TOPRIGHT 3-BOTTOMLEFT 4-BOTTOMRIGHT
		int whichQ = 0;

		std::cout << "build tree - QTOPLEFT" << endl;
		std::cout << "start x " << ctrUl_x << endl;
		std::cout << "start y " << ctrUl_y << endl;
		std::cout << "end x: " << ctrUl_x + width/2 << endl;

		for (int x = ctrUl_x; x < ctrUl_x + width/2; x++) {
			for (int y = ctrUl_y; y < ctrUl_y + height/2; y ++) {
				pair<int, int> seUL = make_pair(x , y);
				pair<int, int> swUL = make_pair((x + width/2) % (width), y);
				pair<int, int> nwUL = make_pair((x + width/2) % (width), (y + height/2) % (height));
				pair<int, int> neUL = make_pair(x,(y + height/2) % (height));
				// CALL avgEnt helper function. (SE, SW, NW, NE)
				avgEntropy = (getQuadEntropy(seUL, s, k) + getQuadEntropy(swUL, s, k) + 
					getQuadEntropy(nwUL, s, k) + getQuadEntropy(neUL, s, k)) / 4;

			// sum 4 for 1 avg entropy.

				if (avgEntropy < minEnt) {
					minEnt = avgEntropy;
					optSplitPos.first = x;
					optSplitPos.second = y;
				}
			// std::cout << "avgEntropy: " << avgEntropy << endl;
			// std::cout << "minEnt: " << minEnt << endl;
			}
			std::cout << "finished one set of x: " << x << endl;
		}
			
		// after 4 for loops - make child image (5 cases based on the splitPoint)

		unsigned int seUlX = optSplitPos.first;
		unsigned int seUlY = optSplitPos.second;
		int parentLen = (int) pow(2, k);
		int childLen = (int) pow(2, k - 1);

		pair<int,int> swUl;
		swUl.first  = (seUlX + childLen) % parentLen;
		swUl.second = seUlY;

		pair<int,int> nwUl;
		nwUl.first = (seUlX + childLen) % parentLen;
		nwUl.second = (seUlY + childLen) % parentLen;

		pair<int,int> neUl;
		neUl.first = seUlX;
		neUl.second = (seUlY + childLen) % parentLen;

		// call makeNewImg helper func
		seChild = makeNewImg(k--, im, optSplitPos);
		swChild = makeNewImg(k--, im, swUl);
		nwChild = makeNewImg(k--, im, nwUl);
		neChild = makeNewImg(k--, im, neUl);
	}

	// for splitting
	// TODO: have to get optSplitPos first 
	// Make 4 PNG im for each child
	Node* node = new Node(optSplitPos, k, avgPixel);

	// Call Recursive func here
	node->NW = buildTree(nwChild, k--);
	node->NE = buildTree(neChild, k--);
	node->SE = buildTree(seChild, k--);
	node->SW = buildTree(swChild, k--);

	return node;
}

/* your code here */

// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to 
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.

PNG toqutree::render(){
	unsigned int width = pow(2, root->dimension);
	unsigned int height =  width;
	PNG toRender(width, height);

	toRender = renderHelper(toRender, root);
	//TODO: to add helper here.. Question: how to connnect RenderHelper with our previous stitchImages????

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

/* your code here */

	return toRender;
}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
	//TODO NOTE: no need to pass in avgCol. just calculate based on HSLA dist.
	// if (root->NE == NULL){ // no children, nothing to prune.
	// 	return; 
	// }
	pruneHelper(root, tol);
}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
/* your code here */
	if (curr == NULL){
		return;
	}
	if (curr->NE != NULL){
		clear(curr->NE);
		clear(curr->NW);
		clear(curr->SE);
		clear(curr->SW);
	}
	delete curr;
	curr = NULL;
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
	if (other == NULL) {
        return NULL;
    }
	Node * newNode = new Node(other->center, other->dimension, other->avg);

	if (other->NW != NULL)
	{
		newNode->NW = copy(other->NW);
		newNode->NE = copy(other->NE);
		newNode->SE = copy(other->SE);
		newNode->SW = copy(other->SW);
	}
	 return newNode;
/* your code here */
}


//** HELPERS****

int toqutree::countSize(Node * root){
	if (root == NULL ){
		return 0;
	} else if (root->NW == NULL){ // will always have 4 child and first child is NW.
		return 1;
	} else {
		return (countSize(root->NW) + countSize(root->NE) + countSize(root->SE) + countSize(root->SW));
	}
}

double toqutree::getQuadEntropy(pair<int, int> quadUL, stats &s, int dim) {
	// e.g test. NE = (3,1) , NW = (7,1),  SE = (3,5), SW = (7,5)
	// std::cout << "k passed is 9 " << dim << endl;
    int twokDim = pow(2, dim); // 8
	int quadArea = (twokDim/2) * (twokDim/2);
	int ulX = quadUL.first; // 3
	int ulY = quadUL.second; // 1
    double quadEnt = 0.0;
	
    // 
	int lrX = ((ulX + (twokDim/2)) % twokDim) - 1; // 6 // NW:2 // SE: (3+4)%8 - 1 = 6 //SW: (7+4 )% 8 - 1 = 2
	if (lrX == -1) {
		lrX = (twokDim - 1);
	}
	int lrY = ((ulY + (twokDim/2)) % twokDim) - 1; // 4 // NW:4 // SE: (5+4)%8 - 1 = 0 //SW: (5+4)% 8 - 1 = 0
	if (lrY == -1) {
		lrY = (twokDim - 1);
	}
	pair<int, int> quadLR = make_pair(lrX, lrY); // NElr: (6,4), NWlr: (2,4), SElr: (6,0), SWlr: (2,0)

    if ((ulX < lrX ) && (ulY < lrY )) { // perfect square (NE) 
	// std::cout << "perfect Square" << endl;
        quadEnt = s.entropy(quadUL, quadLR);

    } else if ((ulX > lrX ) && (ulY < lrY )) {// left + right split. (NW)
		pair<int, int> leftUL = make_pair(0, ulY);
		pair<int, int> leftLR = quadLR;
		pair<int, int> rightUL = quadUL;
        pair<int, int> rightLR = make_pair(twokDim - 1, lrY);
		// std::cout << "left right" << endl;

		quadEnt = ((s.entropy(leftUL, leftLR) * s.rectArea(leftUL, leftLR)) + 
		(s.entropy(rightUL, rightLR) * s.rectArea(rightUL, rightLR))) / quadArea;
		// std::cout << "left right quadEnt " << quadEnt << endl;

	} else if ((ulX < lrX ) && (ulY > lrY )) { // top + bottom split. (SE)
		pair<int, int> topUL = make_pair(ulX, 0); // top part visually
		pair<int, int> topLR = quadLR;
		pair<int, int> lowerUL = quadUL;
        pair<int, int> lowerLR = make_pair(lrX, twokDim - 1);
		// std::cout << "top downt" << endl;

		quadEnt = ((s.entropy(topUL, topLR) * s.rectArea(topUL, topLR)) + 
		(s.entropy(lowerUL, lowerLR) * s.rectArea(lowerUL, lowerLR))) / quadArea;

	} else if ((ulX > lrX) && (ulY > lrY )){ // split 4 ways (SW)
		pair<int, int> upLeftUL = make_pair(0, 0);
		pair<int, int> upLeftLR = quadLR;
        pair<int, int> upRightUL = make_pair(ulX, 0);
		pair<int, int> upRightLR = make_pair(twokDim - 1, lrY);
		pair<int, int> botLeftUL = make_pair(0, ulY);
        pair<int, int> botLeftLR = make_pair(lrX, twokDim - 1);
		pair<int, int> botRightUL = quadUL;
		pair<int, int> botRightLR = make_pair(twokDim - 1, twokDim - 1);

		quadEnt = ((s.entropy(upLeftUL, upLeftLR) * s.rectArea(upLeftUL, upLeftLR)) + 
				(s.entropy(upRightUL, upRightLR) * s.rectArea(upRightUL, upRightLR)) +
				(s.entropy(botLeftUL, botLeftLR) * s.rectArea(botLeftUL, botLeftLR)) +
				(s.entropy(botRightUL, botRightLR) * s.rectArea(botRightUL, botRightLR))) / quadArea;
	}
	// std::cout << "quadEnt CALCULATED! " << quadEnt << endl;
    return quadEnt;
}

PNG toqutree::makeImageNoStitch(int dim, PNG & im, pair<int,int> splitPoint){
	unsigned int width = pow(2,dim);
	unsigned int height = width;
	PNG newIm(width, height);
	for (unsigned int i = 0; i < width; i++){
		for (unsigned int j = 0; j < height; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel(splitPoint.first + i, splitPoint.second + j);
		}
	}
	return newIm;

}

// TODO: return pointer or &???
PNG toqutree::stitchImgVertical(int dim, PNG & im, pair<int,int> splitPoint){
	unsigned int width = pow(2,dim);
	unsigned int height = width;
	PNG newIm(width, height);
	int x = splitPoint.first; // 7
	int y = splitPoint.second; // 7

	// first vertical
	for (unsigned int i = 0; i < width - x; i++){
		for (unsigned int j = 0; j < height; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel(width + x + i, y + j);
		}
	}

	// second vertical
	for (unsigned int i = width - x ; i < (unsigned int) x; i++){
		for (unsigned int j = 0 ; j < height; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel(i, y + j);
		}
	}
	return newIm;

}

PNG toqutree::stitchImgHor(int dim, PNG & im, pair<int,int> splitPoint){

	std::cout << "stitchImgHor !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	int width = pow(2,dim);
	int height = pow(2,dim);
	PNG newIm(width, height);
	int x = splitPoint.first; // 7
	int y = splitPoint.second; // 7

	// first Horizontal (bottom)
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height - y; j++){
			std::cout << "HORIZONTAL X " << x << "HORIZONTAL i "<< i << endl;
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel(x + i, y + height + j);
		}
	}

	// second Horizontal (top)
	for (unsigned int i = 0; i < width; i++){
		for (unsigned int j = height - y; j <(unsigned int) y; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			pixelNew = im.getPixel(x + i, j);
		}
	}

	return newIm;

}

PNG toqutree::stitchImgVandH(int dim, PNG & im, pair<int,int> splitPoint){
	unsigned int width = pow(2,dim);
	unsigned int height = width;
	PNG newIm(width, height);
	int x = splitPoint.first; // 7
	int y = splitPoint.second; // 7

	// #4 bottom right corner
	for (unsigned int i = 0; i < (unsigned int) width - x; i++){
		for (unsigned int j = 0; j < (unsigned int) height - y; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel( x + width + i, y + height + j);
		}
	}

	// #3 bottom left corner
	for (unsigned int i = width - x; i < (unsigned int) x; i++){
		for (unsigned int j = 0; j < (unsigned int) height - y; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel(i, y + height + j);
		}
	}

	// #2 Top right corner
	for (unsigned int i = 0; i < (unsigned int) width - x; i++){
		for (unsigned int j = height - y; j < (unsigned int) y; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel( x + width + i, j);
		}
	}

	// #1 Top left corner
	for (unsigned int i = width - x; i < (unsigned int) x; i++){
		for (unsigned int j = height - y; j < (unsigned int) y; j++){
			HSLAPixel * pixelNew = newIm.getPixel(i, j);
			* pixelNew = *im.getPixel(i, j);
		}
	}

	return newIm;
}

bool toqutree::isMinEntropy(double min, double avg){
	if (avg < min){
		return true;
	}
	return false;
}

// double toqutree::getEntropyTopLeftQ(pair<int, int> curSplitPos, long rectArea, int width, int height, stats s) {
// 	// std::cout << "build tree - get Entropy Top Left Q" << endl;
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;
// 	pair<int,int> lrSe((width/2) + x - 1,(height/2) + y - 1);
// 	double entropySE = s.entropy(curSplitPos, lrSe);

// 	pair<int,int> ulNwTopLeft(0,0);
// 	pair<int,int> lrNwTopLeft(x-1,y-1);
// 	pair<int,int> ulNwTopright(((width/2) + x),0);
//     pair<int,int> lrNwTopright(width-1,y-1);
// 	pair<int,int> ulNwBottomLeft(0, (y+(height/2)));
// 	pair<int,int> lrNwBottomLeft((x-1) , (height -1));
// 	pair<int,int> ulNwBottomRight((width/2)+x, y+(height/2));
// 	pair<int,int> lrNwBottomRight( (width -1), (height -1));
// 	double entropyNW = getEntropyFromFour(ulNwTopLeft, lrNwTopLeft, ulNwTopright, lrNwTopright, ulNwBottomLeft, lrNwBottomLeft, ulNwBottomRight, lrNwBottomRight, s, rectArea);
// 	// std::cout << "entropyNW: " << entropyNW << endl;

// 	pair<int,int> ulSeRight((width/2) + x,y);
// 	pair<int,int> lrSeRight(width -1,(height/2) + y - 1);
// 	pair<int,int> ulSeLeft(0,y);
// 	pair<int,int> lrSeLeft(x-1,(height/2) + y - 1);
// 	double entropySW = getEntropyFromTwo(ulSeRight,lrSeRight,ulSeLeft,lrSeLeft, s, rectArea);


// 	pair<int,int> ulNeTop(x,0);
// 	pair<int,int> lrNeTop((width/2) + x - 1,y-1);
// 	pair<int,int> ulNeBottom(x,y+(height/2));
// 	pair<int,int> lrNeBottom((width/2) + x - 1,height -1);
// 	double entropyNE = getEntropyFromTwo(ulNeTop,lrNeTop,ulNeBottom,lrNeBottom, s, rectArea);

// 	return ((entropySE + entropyNW + entropySW + entropyNE) / 4);
// }

// double toqutree::getEntropyTopRightQNoSplit(pair<int, int> curSplitPos, long rectArea, int width, int height, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;
// 	pair<int,int> lrSe(width -1, y + (height/2) - 1);
// 	double entropySE = s.entropy(curSplitPos, lrSe);

// 	pair<int,int> ulSw(0, y);
// 	pair<int,int> lrSw(x-1, y + (height/2) - 1);
// 	double entropySW = s.entropy(ulSw, lrSw);

// 	pair<int,int> ulNeTop(x, 0);
// 	pair<int,int> lrNeTop(width-1, y - 1);
// 	pair<int,int> ulNeBottom(x, y + (height/2));
// 	pair<int,int> lrNeBottom(width -1, height -1);
// 	double entropyNE = getEntropyFromTwo(ulNeTop,lrNeTop,ulNeBottom,lrNeBottom, s, rectArea);
	
// 	pair<int,int> ulNwTop(0, 0);
// 	pair<int,int> lrNwTop(x-1, y - 1);
// 	pair<int,int> ulNwBottom(0, y + (height/2));
// 	pair<int,int> lrNwBottom(x -1, height -1);
// 	double entropyNW = getEntropyFromTwo(ulNwTop,lrNwTop,ulNwBottom,lrNwBottom, s, rectArea);
	
// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyTopRightQ(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokHeight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;
	
// 	pair<int,int> ulSeRight(x, y);
// 	pair<int,int> lrSeRight((twokDim -1), (y + (twokHeight/2) - 1));
// 	pair<int,int> ulSeLeft(0, y);
// 	pair<int,int> lrSeLeft((x- (twokDim/2) - 1), (y + (twokHeight/2) - 1));

// 	double entropySE = getEntropyFromTwo(ulSeRight,lrSeRight,ulSeLeft,lrSeLeft, s, rectArea);

// 	pair<int,int> ulSw(x- (twokDim/2), y);
// 	pair<int,int> lrSw(x-1, y + (twokHeight/2) - 1);
// 	double entropySW = s.entropy(ulSw,lrSw);

// 	pair<int,int> ulNwTop(x- (twokDim/2), 0);   // (x,y) = (10,6)
// 	pair<int,int> lrNwTop(x-1, y-1);
// 	pair<int,int> ulNwBottom(x- (twokDim/2), y+(twokHeight/2));
// 	pair<int,int> lrNwBottom(x-1, twokHeight- 1);
// 	double entropyNW = getEntropyFromTwo(ulNwTop,lrNwTop, ulNwBottom, lrNwBottom, s, rectArea);

// 	pair<int,int> ulNeTopRight(x, 0);   
// 	pair<int,int> lrNeTopRight(twokDim-1, y-1);
// 	pair<int,int> ulNeBottomRight(x, y+(twokHeight/2));
// 	pair<int,int> lrNeBottomRight(twokDim-1, twokHeight- 1);
// 	pair<int,int> ulNeTopLeft(0, 0);   
// 	pair<int,int> lrNeTopLeft(x-(twokDim/2)-1, y-1);
// 	pair<int,int> ulNeBottomLeft(0, y+(twokHeight/2));
// 	pair<int,int> lrNeBottomLeft(x-(twokDim/2)-1, twokHeight-1);
// 	double entropyNE = getEntropyFromFour(ulNeTopRight, lrNeTopRight, ulNeBottomRight, lrNeBottomRight, ulNeTopLeft, lrNeTopLeft, ulNeBottomLeft, lrNeBottomLeft, s, rectArea);

// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyFromFour(pair<int,int> ulTopRight, pair<int,int> lrTopRight, pair<int,int> ulBottomRight,
// pair<int,int> lrBottomRight, pair<int,int> ulTopLeft, pair<int,int> lrTopLeft, pair<int,int> ulBottomLeft,pair<int,int> lrBottomLeft, stats s, long rectArea) {
// 	double entropy = ((s.entropy(ulTopLeft, lrTopLeft) * s.rectArea(ulTopLeft, lrTopLeft))
// 						+ (s.entropy(ulTopRight, lrTopRight) * s.rectArea(ulTopRight, lrTopRight))
// 						+ (s.entropy(ulBottomLeft, lrBottomLeft) * s.rectArea(ulBottomLeft, lrBottomLeft))
// 						+ (s.entropy(ulBottomRight, lrBottomRight) * s.rectArea(ulBottomRight, lrBottomRight)))
// 						/rectArea;
// 	return entropy;
// }

// double toqutree::getEntropyFromTwo(pair<int,int> ulTop, pair<int,int> lrTop, pair<int,int> ulBottom,
// pair<int,int> lrBottom, stats s, long rectArea) {
// 	double entropy = (((s.entropy(ulTop,lrTop) * (s.rectArea(ulTop,lrTop)))
// 					  + (s.entropy(ulBottom,lrBottom) * (s.rectArea(ulBottom,lrBottom))))
// 					  / rectArea);

// 	return entropy;
// }

// double toqutree::getEntropyBottomRightQfourPerfectSquares(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokheight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;
// 	pair<int,int> ulSe = curSplitPos;
// 	pair<int,int> lrSe(twokDim-1, twokheight-1);
// 	double entropySE = s.entropy(ulSe, lrSe);

// 	pair<int,int> ulSw(0, y);
// 	pair<int,int> lrSw(x-1, twokheight-1);
// 	double entropySW = s.entropy(ulSw, lrSw);

// 	pair<int,int> ulNE(x, 0);
// 	pair<int,int> lrNE(twokDim-1, y-1);
// 	double entropyNE = s.entropy(ulNE, lrNE);

	
// 	pair<int,int> ulNW(0, 0);
// 	pair<int,int> lrNW(x-1, y - 1);
// 	double entropyNW = s.entropy(ulNW, lrNW);
	
// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyBottomRighNENWNoSplits(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokheight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;
// 	pair<int,int> ulNE(x, y - (twokheight/2));
// 	pair<int,int> lrNE(twokDim-1, y -1);
// 	double entropyNE = s.entropy(ulNE, lrNE);

// 	pair<int,int> ulNW(0, y - (twokheight/2));
// 	pair<int,int> lrNW(x-1, y - 1);
// 	double entropyNW = s.entropy(ulNW, lrNW);

// 	pair<int,int> ulSWTop(0, 0);
// 	pair<int,int> lrSWTop(x-1, y - (twokheight/2));
// 	pair<int,int> ulSWBottom(0, y);
// 	pair<int,int> lrSWBottom(x-1, twokheight -1);
// 	double entropySW = getEntropyFromTwo(ulSWTop,lrSWTop,ulSWBottom,lrSWBottom, s, rectArea);
	
// 	pair<int,int> ulSETop(x, 0);
// 	pair<int,int> lrSETop(twokDim-1, y - (twokheight/2) - 1);
// 	pair<int,int> ulSEBottom(x, y);
// 	pair<int,int> lrSEBottom(twokDim-1, twokheight -1);
// 	double entropySE = getEntropyFromTwo(ulSETop,lrSETop,ulSEBottom,lrSEBottom, s, rectArea);
	
// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyBottomRighNWSWNoSplits(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokheight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;


// 	pair<int,int> ulNW(x - (twokDim/2), y - (twokDim/2)); // y will be 0
// 	pair<int,int> lrNW(x-1, y - 1);
// 	double entropyNW = s.entropy(ulNW, lrNW);

// 	pair<int,int> ulSW(x - (twokDim/2), y);
// 	pair<int,int> lrSW(x-1, twokheight - 1);
// 	double entropySW = s.entropy(ulSW, lrSW);

// 	pair<int,int> ulNELeft(0, 0);
// 	pair<int,int> lrNELeft(x - (twokDim/2) - 1 , y -1);
// 	pair<int,int> ulNERight(x, 0);
// 	pair<int,int> lrNERight(twokDim-1, y -1);
// 	double entropyNE = getEntropyFromTwo(ulNELeft,lrNELeft,ulNERight,lrNERight, s, rectArea);

	
// 	pair<int,int> ulSELeft(0, y);
// 	pair<int,int> lrSELeft(x - (twokDim/2) - 1, twokheight - 1);
// 	pair<int,int> ulSERight(x, y);
// 	pair<int,int> lrSERight(twokDim-1, twokheight -1);
// 	double entropySE = getEntropyFromTwo(ulSELeft,lrSELeft,ulSERight,lrSERight, s, rectArea);
	
// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyBottomRighOnlyNWnoSplit(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokheight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;


// 	pair<int,int> ulNW(x - (twokDim/2), y - (twokheight/2));
// 	pair<int,int> lrNW(x-1, y - 1);
// 	double entropyNW = s.entropy(ulNW, lrNW);

// 	pair<int,int> ulSWTop(x - (twokDim/2), 0);
// 	pair<int,int> lrSWTop(x-1, y - (twokheight/2) - 1);
// 	pair<int,int> ulSWBottom(x - (twokDim/2), y);
// 	pair<int,int> lrSWBottom(x - 1, twokheight - 1);
// 	double entropySW = getEntropyFromTwo(ulSWTop,lrSWTop,ulSWBottom,lrSWBottom, s, rectArea);

// 	pair<int,int> ulNELeft(0, y - (twokheight/2));
// 	pair<int,int> lrNELeft(x - (twokDim/2) - 1 , y -1);
// 	pair<int,int> ulNERight(x, y - (twokheight/2));
// 	pair<int,int> lrNERight(twokDim-1, y -1);
// 	double entropyNE = getEntropyFromTwo(ulNELeft,lrNELeft,ulNERight,lrNERight, s, rectArea);

	
// 	pair<int,int> ulSETopLeft(0, 0);   
// 	pair<int,int> lrSETopLeft(x-(twokDim/2)-1, y - (twokheight/2)-1);
// 	pair<int,int> ulSETopRight(x, 0);   
// 	pair<int,int> lrSETopRight(twokDim-1,  y - (twokheight/2)-1);
// 	pair<int,int> ulSEBottomLeft(0, y);
// 	pair<int,int> lrSEBottomLeft(x-(twokDim/2)-1, twokheight-1);
// 	pair<int,int> ulSEBottomRight(x, y);
// 	pair<int,int> lrSEBottomRight(twokDim-1, twokheight- 1);
// 	double entropySE = getEntropyFromFour(ulSETopLeft, lrSETopLeft, ulSETopRight, lrSETopRight, ulSEBottomLeft, lrSEBottomLeft, 
// 	ulSEBottomRight, lrSEBottomRight, s, rectArea);
	
// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyBottomLeftQNoSplit(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokheight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;

// 	pair<int,int> ulSE(x, y);   
// 	pair<int,int> lrSE(x+(twokDim/2)-1, twokheight -1);
// 	double entropySE = s.entropy(ulSE, lrSE);

// 	pair<int,int> ulNE(x, 0);   
// 	pair<int,int> lrNE(x+(twokDim/2)-1, y-1);
// 	double entropyNE = s.entropy(ulNE,lrNE);

// 	pair<int,int> ulNWLeft(x+(twokDim/2), 0);   
// 	pair<int,int> lrNWLeft(twokDim -1, y-1);
// 	pair<int,int> ulNWRight(0, 0);   
// 	pair<int,int> lrNWRight(x-1, y -1);
// 	double entropyNW = getEntropyFromTwo(ulNWLeft,lrNWLeft,ulNWRight,lrNWRight,s,rectArea);

// 	pair<int,int> ulSWLeft(x+(twokDim/2), y);   
// 	pair<int,int> lrSWLeft(twokDim -1, twokheight -1);
// 	pair<int,int> ulSWRight(0, y);   
// 	pair<int,int> lrSWRight(x-1, twokheight -1);
// 	double entropySW = getEntropyFromTwo(ulSWLeft,lrSWLeft,ulSWRight,lrSWRight,s,rectArea);

// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;
// }

// double toqutree::getEntropyBottomLeftQ(pair<int, int> curSplitPos, long rectArea, int twokDim, int twokheight, stats s) {
// 	int x = curSplitPos.first;
// 	int y = curSplitPos.second;

// 	pair<int,int> ulSETop(x, 0);   
// 	pair<int,int> lrSETop(x+(twokDim/2) -1, y-(twokheight/2) -1);
// 	pair<int,int> ulSEBottom(x, y);   
// 	pair<int,int> lrSEBottom(x+(twokDim/2) -1, twokheight -1);
// 	double entropySE = getEntropyFromTwo(ulSETop,lrSETop,ulSEBottom,lrSEBottom,s,rectArea);

// 	pair<int,int> ulNE(x, y-(twokheight/2));   
// 	pair<int,int> lrNE(x+(twokDim/2) -1, y-1);
// 	double entropyNE = s.entropy(ulNE,lrNE);

// 	pair<int,int> ulNWLeft(0, y-(twokheight/2));   
// 	pair<int,int> lrNWLeft(x-1, y-1);
// 	pair<int,int> ulNWRight(x+(twokDim/2), y-(twokheight/2));   
// 	pair<int,int> lrNWRight(twokDim -1, y-1);
// 	double entropyNW = getEntropyFromTwo(ulNWLeft,lrNWLeft,ulNWRight,lrNWRight,s,rectArea);

// 	pair<int,int> ulSWTopLeft(0, 0);   
// 	pair<int,int> lrSWTopLeft(x-1, y-(twokheight/2) -1);
// 	pair<int,int> ulSWTopRight(x+(twokDim/2), 0);   
// 	pair<int,int> lrSWTopRight(twokDim-1, y-(twokheight/2) -1);
// 	pair<int,int> ulSWBottomLeft(0, y);   
// 	pair<int,int> lrSWBottomLeft(x-1, twokheight -1);
// 	pair<int,int> ulSWBottomRight(x+(twokDim/2), y);   
// 	pair<int,int> lrSWBottomRight(twokDim-1, twokheight -1);
// 	double entropySW = getEntropyFromFour(ulSWTopLeft,lrSWTopLeft,ulSWTopRight,lrSWTopRight,ulSWBottomLeft,lrSWBottomLeft,ulSWBottomRight,lrSWBottomRight,s,rectArea);

// 	return (entropySE + entropyNW + entropySW + entropyNE) / 4;

// 	}


PNG toqutree::renderHelper(PNG & toRender, Node * root){
	//TODO!!!
	return toRender;
}

void toqutree::pruneHelper(Node * & aNode, double tol) {
	std::cout << "prunehelper" << endl;
	if (aNode == NULL){ // no children, nothing to prune.
		return;
	}
	HSLAPixel avgToCheck = aNode->avg;
	if (withinTolerance(aNode, tol, avgToCheck)) {
		std::cout << "clearing" << endl;
		clear(aNode->NE);
		clear(aNode->NW);
		clear(aNode->SE);
		clear(aNode->SW);
	} else {
		std::cout << "else prune" << endl;
		pruneHelper(aNode->NE,tol);
		pruneHelper(aNode->NW,tol);
		pruneHelper(aNode->SE,tol);
		pruneHelper(aNode->SW,tol);
	}	
}

bool toqutree::withinTolerance(Node * node,  double tol, HSLAPixel nodePixel){
	// bool withinTol = false;
	std::cout << "calculating" << endl;
	if (node->NE == NULL){
		if ((((node->avg).dist(nodePixel)) <= tol)){
			return true;
		}
		return false;
	}

	return (withinTolerance(node->NE, tol, nodePixel) && withinTolerance(node->NW, tol, nodePixel)
	&& withinTolerance(node->SE, tol, nodePixel) && withinTolerance(node->SW, tol, nodePixel));
}


PNG toqutree::makeNewImg(int subImgK, PNG & im, pair<int,int> ul) {
	unsigned int width = pow(2,subImgK);
	unsigned int height = width;
	PNG newIm(width, height);

    for (unsigned int i = 0; i < width; i++) {
        for (unsigned int j = 0; j < height; j++) {
			int x = (ul.first + j) % width;
			int y = (ul.second + i) % height;

            HSLAPixel* pixelNew = newIm.getPixel(i, j);
            *pixelNew = *im.getPixel(x, y);
        }
    }
    return newIm;
}




