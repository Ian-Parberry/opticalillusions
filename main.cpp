/// \file main.cpp

/// \brief Generate a pair of optical illusions in SVG format.

// MIT License
//
// Copyright (c) 2021 Ian Parberry
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <string>

const float PI = 3.14159265358979323846f; ///< Pi.

//////////////////////////////////////////////////////////////////////////
// Helper fuctions.

#pragma region helpers

/// \brief Open SVG file.
///
/// Open an SVG file for writing and print the header tag and an
/// open `svg` tag.
/// \param output [out] Reference to output file pointer.
/// \param fname File name without extension.
/// \param w Image width.
/// \param h Image height.
/// \return true if open succeeded.

bool OpenSVG(FILE*& output, const std::string& fname, size_t w, size_t h){
  const std::string s = fname + ".svg";
  
#ifdef _MSC_VER //Visual Studio 
  fopen_s(&output, s.c_str(), "wt");
#else
  output = fopen(s.c_str(), "wt");
#endif
  
  if(output != nullptr){ //write header to file
    fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"); //xml tag

    fprintf(output, "<svg width=\"%lu\" height=\"%lu\" ", w, w); //svg tag
    fprintf(output, "viewBox=\"0 0 %lu %lu\" ", w, w);
    fprintf(output, "xmlns=\"http://www.w3.org/2000/svg\">\n");
    
    fprintf(output, "<!-- Created by Ian Parberry -->\n"); //author comment
     
    return true; //success
  } //if

  return false; //failure
} //OpenSVG

/// \brief Close SVG file.
///
/// Print a close `svg` tag and close the SVG file.
/// \param output Reference to file pointer.

void CloseSVG(FILE*& output){
  if(output != nullptr){
    fprintf(output, "</svg>\n"); //close the svg tag
    fclose(output);
    output = nullptr;
  } //if
} //CloseSVG

#pragma endregion helpers

//////////////////////////////////////////////////////////////////////////
// Optical Illusion 1 - circles of squares.

#pragma region Illusion1

/// \brief Draw a circle of squares to a file in SVG format.
/// 
/// This function outputs SVG `transform` and SVG `rect` tags to the output
/// file, alternating between black and white. The squares are spaced apart by
/// approximately half a square width and tilted slightly from the perpendicular
/// to a line drawn from the center of the circle to the center of the square.
/// The number of squares is chosen so as to fit the spacing constraint, 
/// which need not be exact for the optical illusion to work.
/// Used for optical illusion 1.
///
/// \image html OneRingOfSquares.svg height=240
///
/// \param output File pointer.
/// \param cx Image center x.
/// \param cy Image center y.
/// \param r Circle radius in pixels.
/// \param sw Square width and height.
/// \param parity Square initial orientation parity.

void DrawCircleOfSquares(FILE* output, size_t cx, size_t cy, float r,
  size_t sw, bool parity)
{
  //number of squares on circle, must be even
  const size_t n = (size_t)ceil((2*PI*r)/(1.5f*sw)) & 0xFFFFFFFE; 

  const float dtheta = 2*PI/n; //angle delta to next square
  float theta = 0; //angle to current square

  for(size_t i=0; i<n; i++){ //for each square
    const float x = r*cosf(theta); //square center x
    const float y = r*sinf(theta); //square center y
    const float phi = 12*(parity? 1: -1) + 180*theta/PI; //square orientation

    fprintf(output, "<g transform=\"translate(%0.1f %0.1f)", x + sw/2, y + sw/2); //translate
    fprintf(output, "rotate(%0.1f %lu %lu)\">", phi, cx, cy); //rotate
    fprintf(output, "<rect width=\"%lu\" height=\"%lu\" ", sw, sw); //rectangle

    if(i&1)fprintf(output, "class=\"b\""); //black
    else fprintf(output, "class=\"w\""); //white

    fprintf(output, "/>"); //close rect tag
    fprintf(output, "</g>\n"); //close group

    theta += dtheta; //next square
  } //for
} //DrawCircleOfSquares

/// \brief Draw the first optical illusion to a file in SVG format.
/// 
/// The image consists of four concentric circles of tilted squares,
/// alternating between light and dark squares. This function outputs an SVG
/// `style` tag (the use of which refices the SVG file size) and the 
/// background `rectangle` tag, then calls DrawCircleOfSquares()
/// once for each circle of squares required.
///
/// \image html output1.svg height=250
/// 
/// \param fname File name without extension.
/// \param w Width and height of image in pixels.
/// \param n Number of circles.
/// \param r0 Initial circle radius.
/// \param dr Radius delta.
/// \param sw Width of squares.
/// \param dark A dark SVG color.
/// \param light A light SVG color.
/// \param bgclr A mid-range SVG color for the background.

void OpticalIllusion1(const std::string& fname, size_t w, size_t n,
  float r0, float dr, size_t sw, const char dark[], const char light[],
  const char bgclr[])
{
  const size_t cx = w/2 - sw/2; //center x coordinate
  const size_t cy = cx; //center y coordinate
  FILE* output = nullptr; //output file pointer

  if(OpenSVG(output, fname, w, w)){
    //style tag
    fprintf(output, "<style>"); //open style tag
    fprintf(output, "rect{fill:none;stroke-width:3}"); //rectangle
    fprintf(output, "rect.b{x:%lu;y:%lu;stroke:%s;}", cx, cy, dark); //black rect
    fprintf(output, "rect.w{x:%lu;y:%lu;stroke:%s;}", cx, cy, light); //white rect
    fprintf(output, "</style>\n"); //close style tag
    
    //background
    fprintf(output, "<rect width=\"%lu\" height=\"%lu\" ", w, w); //rectangle
    fprintf(output, "style=\"fill:%s\"/>\n", bgclr); //fill
  
    for(size_t i=0; i<n; i++) //for each circle of squares
      DrawCircleOfSquares(output, cx, cy, r0 + i*dr, sw, i&1); //draw it
       
    CloseSVG(output); //clean up and exit
  } //if
} //OpticalIllusion1

#pragma endregion Illusion1

//////////////////////////////////////////////////////////////////////////
// Optical Illusion 2 - circles of circles of ellipses.

#pragma region Illusion2

/// \brief Select ellipse color based on index.
/// 
/// If parity is true, ellipse is black when i%4=0, white when ji%4==2, and 
/// blank when i%4==1 and i%4==3. If parity is false, black and white are
/// flipped. This function outputs the appropriate class name, `class="b"` for
/// black and `class="w"` for white, to the output file. Used for optical
/// illusion 2.
/// 
/// \param output Output file pointer.
/// \param i Ellipse index about circle.
/// \param parity True if first ellipse is black, false if white.

void SelectEllipseColor(FILE* output, size_t i, bool parity){
  const size_t j = i%4;
  if((parity && j == 0) || (!parity && j == 2))
    fprintf(output, "class=\"b\""); //black ellipse
  else if((parity && j == 2) || (!parity && j == 0))
    fprintf(output, "class=\"w\""); //white ellipse
} //SelectEllipseColor

/// \brief Draw circle of ellipses to a file in SVG format.
/// 
/// Draw a circle of elipses oriented so that the long axis of each ellipse is
/// perpendicular to a line drawn from the center of the circles to the center
/// of the ellipse. This function outputs SVG `transform` and SVG `ellipse` tags
/// to the output file. Used for optical illusion 2.
/// 
/// \param output Output file pointer.
/// \param cx X coordinate of center of image in pixels.
/// \param cy Y coordinate of center of image in pixels.
/// \param r Radius of circle.
/// \param r0 Long radius of ellipses.
/// \param r1 Short radius of ellipses.
/// \param n Number of ellipses in ring.
/// \param theta Angle to first ellipse.
/// \param dtheta Angle delta.
/// \param parity True if first ellipse is black, false if white.
/// \param flip True to clip the ordering of colots of ellipses.

void DrawCircleOfEllipses(FILE* output, size_t cx, size_t cy, float r,
  float r0, float r1, size_t n, float theta, float dtheta, bool parity,
  size_t flip=999999)
{
  for(size_t i=0; i<n; i++){ //for each ellipse
    const float x = r*cosf(theta); //ellipse center x
    const float y = r*sinf(theta); //ellipse center y
    const float phi = 90 + 180*theta/PI; //ellipse orientation

    fprintf(output, "<g transform=\"translate(%0.1f %0.1f)", x, y); //translate
    fprintf(output, "rotate(%0.1f %lu %lu)\">", phi, cx, cy); //rotate
    fprintf(output, "<ellipse rx=\"%0.1f\" ry=\"%0.1f\" ", r0, r1); //ellipse
    
    SelectEllipseColor(output, i, parity);

    fprintf(output, "/>"); //close ellipse tag
    fprintf(output, "</g>\n"); //close group

    theta += dtheta; //next ellipse
    if(i == flip)parity = !parity; //flip parity if we need to
  } //for
} //DrawCircleOfEllipses

/// \brief Draw 3 concentric circles of ellipses to a file in SVG format.
/// 
/// This function calls DrawCircleOfEllipses() three times, once for each
/// circle of ellipses. The middle circle is drawn first, then
/// the inner circle, then the outer circle. The parameters for the calls 
/// DrawCircleOfEllipses() are chosen so as to achieve the following.
/// 
/// The inner circle starts with a black ellipse centered at the top and
/// alternates with white ellipses each spaced roughly one ellipse long-axis
/// apart for a total of 36 ellipses as shown in the next image.
/// 
/// \image html ring1-middle.svg height=250
///
/// The inner circle also has 36 ellipses, but it starts with a gap centered
/// at the top with black ellipses to the left and right and a gap centered at
/// the bottom with white ellipses to the left and right. Black and white
/// alternate for the rest of the circle.
/// 
/// \image html ring2-inner.svg height=250
///
/// The outer circle of a ring is similar to the inner circle
/// but has black and white interchanged.
/// 
/// \image html ring3-outer.svg height=250
/// 
/// Used for optical illusion 2.
/// 
/// \param output Pointer to output file.
/// \param cx X coordinate of center of image in pixels.
/// \param cy Y coordinate of center of image in pixels.
/// \param r Radius of braid.
/// \param r0 Long radius of ellipses.
/// \param r1 Short radius of ellipses.
/// \param n Number of ellipses in ring.
/// \param flip True to flip the ordering of colors of ellipses.

void DrawTripleCircle(FILE* output, size_t cx, size_t cy, float r,
  float r0, float r1, size_t n, bool flip=false)
{
  const float dtheta = PI/n; //angle delta to next ellipse
  float theta = (flip? PI: -PI)/2; //angle to next ellipse
  n *= 2; //include spaces

  DrawCircleOfEllipses(output, cx, cy, r, r0, r1, n, theta, dtheta, true);  
  DrawCircleOfEllipses(output, cx, cy, r - r1, r0, r1, n, 
    theta + dtheta, dtheta, true, n/2 - 1);  
  DrawCircleOfEllipses(output, cx, cy, r + r1, r0, r1, n, 
    theta + dtheta, dtheta, false, n/2 - 2);
} //DrawTripleCircle

/// \brief Draw the second optical illusion to a file in SVG format.
/// 
/// The image consists of a pair of concentric rings, each of which is made
/// up of three concentric circles of ellipses. This function outputs an SVG
/// `style` tag (the use of which refices the SVG file size) and the 
/// background `rectangle` tag, then calls DrawTripleCircle()
/// twice, once for each triplet of circles.
///
/// \image html output2.svg height=250
/// 
/// \param fname File name without extension.
/// \param w Width and height of image in pixels.
/// \param n Number of ellipses in ring.
/// \param r Radius of braid.
/// \param r0 Long radius of ellipses.
/// \param r1 Short radius of ellipses.
/// \param dark A dark SVG color.
/// \param light A light SVG color.
/// \param bgclr A mid-range SVG color for the background.

void OpticalIllusion2(const std::string& fname, size_t w, size_t n,
  float r, float r0, float r1, const char dark[], const char light[],
  const char bgclr[])
{
  const size_t cx = w/2; //center x coordinate
  const size_t cy = cx; //center y coordinate
  FILE* output = nullptr; //output file pointer

  if(OpenSVG(output, fname, w, w)){
    //style tag
    fprintf(output, "<style>"); //open style tag
    fprintf(output, "ellipse{fill:none;stroke-width:3}");	//ellipse
    fprintf(output, "ellipse.b{cx:%lu;cy:%lu;stroke:none;fill:%s;}",
      cx, cy, dark);	//dark ellipse
    fprintf(output, "ellipse.w{cx:%lu;cy:%lu;stroke:none;fill:%s;}",
      cx, cy, light);	//light ellipse
    fprintf(output, "</style>\n"); //close style tag
    
    //background
    fprintf(output, "<rect width=\"%lu\" height=\"%lu\" ", w, w); //rectangle
    fprintf(output, "style=\"fill:%s\"/>\n", bgclr); //fill
  
    DrawTripleCircle(output, cx, cy, r, r0, r1, 36);
    DrawTripleCircle(output, cx, cy, r - 64, 0.8f*r0, 0.8f*r1, 36, true);

    CloseSVG(output); //clean up and exit
  } //if
} //OpticalIllusion2

#pragma region Illusion2

//////////////////////////////////////////////////////////////////////////

/// \brief Main.
/// 
/// Create two optical illusions and save them as SVG files. The actual work is
/// done by functions OpticalIllusion1() and OpticalIllusion2(), called with
/// various parameters.
/// \return 0.

int main(){
  OpticalIllusion1("output1", 800, 4, 100.0f, 72.0f, 24,
    "black", "white", "gray");
  OpticalIllusion1("output1a", 800, 4, 100.0f, 72.0f, 24,
    "blue", "yellow", "forestgreen");
  OpticalIllusion2("output2", 800, 3, 300.0f, 12.0f, 6.0f,
    "black", "white", "gray");
  OpticalIllusion2("output2a", 800, 3, 300.0f, 12.0f, 6.0f,
    "blue", "yellow", "forestgreen");

  return 0;
} //main