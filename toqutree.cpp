
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
// std::cout << "making tree now" << endl;
// std::cout << "our original dim is" << k << endl;
	if (imIn.width() == pow(2,k) && imIn.height() == pow(2,k)){
		// std::cout << "going to build tree here! imIn height: " << imIn.height() << "k: " << k << endl;
		// std::cout << "going to build tree here! imIn width: " << imIn.width() << "k: " << k << endl;
		root = buildTree(imIn, k);
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
		// std::cout << "going to build tree 2" << endl;
		// std::cout << "subimage passed: " << ctrPoint.first << " " << ctrPoint.second << endl;
		root = buildTree(subIm, k);
	}

}

int toqutree::size() {
	return countSize(root);
/* your code here */
}


toqutree::Node * toqutree::buildTree(PNG & im, int k) {
	// std::cout << "our original dim is " << k << endl;
	// k is big sub-image
	// std::cout << "entered BuildTree when k is" << k << endl;
	stats s(im);
	int width = im.width();
	// std::cout << "entered BuildTree when width is" << width << endl;
	int height = im.height();
	pair<int,int> ul(0,0);
	pair<int,int> lr(width - 1, height - 1);
	HSLAPixel avgPixel = s.getAvg(ul,lr);
	pair<int,int> optSplitPos;
	// Base cases
	if (k == 0){
		// std::cout << "BuildTree when k = 0" << endl;
		Node* node = new Node(ul, k, avgPixel);
		node->SE = NULL;
		node->SW = NULL;
		node->NE = NULL;
		node->NW = NULL;
		return node;
	}

	PNG nwChild(width/2, height/2);
	PNG seChild(width/2, height/2);
	PNG swChild(width/2, height/2);
	PNG neChild(width/2, height/2);
	// std::cout << "BuildTree before if k = 1" << endl;
	if (k == 1){
		// std::cout << "BuildTree when k = 1 finally !!" << endl;
		HSLAPixel * pixelNewNW = nwChild.getPixel(0, 0);
		// std::cout << "getpixel nwChild !!" << endl;
		*pixelNewNW = *im.getPixel(0,0);
		// std::cout << "getpixel original img !!" << endl;

		HSLAPixel * pixelNewSe = seChild.getPixel(0, 0);
		pixelNewSe = im.getPixel(1,1);

		HSLAPixel * pixelNewSw = swChild.getPixel(0, 0);
		pixelNewSw = im.getPixel(0,1);
		
		HSLAPixel * pixelNewNe = neChild.getPixel(0, 0);
		pixelNewNe = im.getPixel(1,0);


	}

	
	else {
		// std::cout << "build tree - check else" << endl;
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

		// std::cout << "build tree - round" << endl;
		// std::cout << "start x " << ctrUl_x << endl;
		// std::cout << "start y " << ctrUl_y << endl;
		// std::cout << "end x: " << ctrUl_x + width/2 << endl;

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
			// std::cout << "finished one set of x: " << x << endl;
		}
			
		// after 4 for loops - make child image (5 cases based on the splitPoint)

		// std::cout << "opt split x " << optSplitPos.first << endl;
		// std::cout << "opt split y " << optSplitPos.second << endl;
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
		int nextK = k - 1;
		seChild = makeNewImg(nextK, im, optSplitPos);
		swChild = makeNewImg(nextK, im, swUl);
		nwChild = makeNewImg(nextK, im, nwUl);
		neChild = makeNewImg(nextK, im, neUl);
	}

	// for splitting
	// TODO: have to get optSplitPos first 
	// Make 4 PNG im for each child
	// std::cout << "our avgPixel is " << avgPixel << endl;
	
	Node* node = new Node(optSplitPos, k, avgPixel);
	// std::cout << "our node avg is " << node->avg << endl;

	// Call Recursive func here
	int nextK = k -1;
	node->NW = buildTree(nwChild, nextK);
	node->NE = buildTree(neChild, nextK);
	node->SE = buildTree(seChild, nextK);
	node->SW = buildTree(swChild, nextK);

	// Node* node = new Node(optSplitPos, k, avgPixel);
	// std::cout << "our node dim is " << k << endl;
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
	unsigned int squareLen = pow(2, root->dimension);
	PNG resultImg(squareLen, squareLen);
	std::cout << "call reder Helper now! " << endl;
	// std::cout << "our root dim is "<< root->dimension << endl;
	resultImg = renderHelper(resultImg, root, root->dimension);
// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

/* your code here */
	return resultImg;
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
	newNode->NW = copy(other->NW);
	newNode->NE = copy(other->NE);
	newNode->SE = copy(other->SE);
	newNode->SW = copy(other->SW);
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

bool toqutree::isMinEntropy(double min, double avg){
	if (avg < min){
		return true;
	}
	return false;
}

PNG toqutree::renderHelper(PNG & resultImg, Node * root, int dim){
	std::cout << "start the render helper " << endl;
	std::cout << "dim is " << dim << endl;
	int squareLen = pow(2, dim);
	std::cout << "squareLen " << squareLen << endl;
	int nodeSquareLen = pow(2, dim -1);
	std::cout << "nodeSquareLen " << nodeSquareLen << endl;
	for(int i = 0; i < squareLen; i++) {
		for(int j =0; j < squareLen; j++) {
			HSLAPixel* pixelNew = resultImg.getPixel(i, j);
			*pixelNew = findPixel(root, dim, i, j);
		}
	}
	std::cout << "finish renderHelper " << endl;
	return resultImg;
}

HSLAPixel toqutree::findPixel(Node* root, int dim, int x, int y) {
	if(root->NE == NULL) {
		// std::cout << "I reached the final pixel leave " << root->avg << endl;
		return root->avg;
	}
	else {
		pair<int, int> splitPos = root->center; 
		unsigned int squareLen = pow(2, dim);
		unsigned int nodeSquareLen = pow(2, dim -1);
		if((splitPos.first <= x || x <= (splitPos.first + nodeSquareLen -1) % squareLen)
			&& (splitPos.second <= y || y <= (splitPos.second + nodeSquareLen -1) % squareLen)) {
			return findPixel(root->SE, dim -1, x, y);
		}
		else if ((splitPos.first <= x || x <= (splitPos.first + nodeSquareLen -1) % squareLen)
		&& (y < splitPos.second || (splitPos.second + nodeSquareLen -1) % squareLen < y)) {
			return findPixel(root->NE, dim -1, x, y);
		}
		else if ((x < splitPos.first || (splitPos.first + nodeSquareLen -1) % squareLen < x)
		&& (splitPos.second <= y || y <= (splitPos.second + nodeSquareLen -1) % squareLen)) {
			return findPixel(root->SW, dim -1, x, y);
		}
		else {
			return findPixel(root->NW,dim -1, x, y);
		}
	}
}

// pair<int,int> splitPos = root->center;
			// if (root->NE == NULL) {
			// 	HSLAPixel* pixelNew = resultImg.getPixel(i, j);
            // 	*pixelNew = root->avg;
			// } else {
			// 	if((splitPos.first <= i || i <= (splitPos.first + nodeSquareLen -1) % squareLen)
			// 		&& (splitPos.second <= j || i <= (splitPos.second + nodeSquareLen -1) % squareLen)) {
					
			// 		}
			// }
void toqutree::pruneHelper(Node * & aNode, double tol) {
	// std::cout << "prunehelper" << endl;
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