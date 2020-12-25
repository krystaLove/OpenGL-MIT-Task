#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        std::cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    Curve curve;
    curve.reserve(P.size() / 4 + 1);

    for (int pointIdx = 0; pointIdx * 3 + 3 < P.size(); ++pointIdx) {
     

        for (int it = 0; it < steps; ++it) {
            CurvePoint point;

            float t = static_cast<float>(it) / steps;

            point.V = Vector3f(
                bezier(P[pointIdx * 3].x(), P[pointIdx * 3 + 1].x(), P[pointIdx * 3 + 2].x(), P[pointIdx * 3 + 3].x(), t),
                bezier(P[pointIdx * 3].y(), P[pointIdx * 3 + 1].y(), P[pointIdx * 3 + 2].y(), P[pointIdx * 3 + 3].y(), t),
                bezier(P[pointIdx * 3].z(), P[pointIdx * 3 + 1].z(), P[pointIdx * 3 + 2].z(), P[pointIdx * 3 + 3].z(), t)
            );

            point.T = Vector3f(
                bezierTangent(P[pointIdx * 3].x(), P[pointIdx * 3 + 1].x(), P[pointIdx * 3 + 2].x(), P[pointIdx * 3 + 3].x(), t),
                bezierTangent(P[pointIdx * 3].y(), P[pointIdx * 3 + 1].y(), P[pointIdx * 3 + 2].y(), P[pointIdx * 3 + 3].y(), t),
                bezierTangent(P[pointIdx * 3].z(), P[pointIdx * 3 + 1].z(), P[pointIdx * 3 + 2].z(), P[pointIdx * 3 + 3].z(), t)
            ).normalized();

            if (pointIdx + it == 0) {
                point.N = Vector3f::cross(Vector3f(0, 0, 1), point.T).normalized();
            } else {
                point.N = Vector3f::cross(curve[steps * pointIdx + it - 1].B, point.T).normalized();
            }

            point.B = Vector3f::cross(point.T, point.N).normalized();

            curve.push_back(point);
        }   
    }

    curve.push_back(curve[0]);

    std::cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    std::cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        std::cerr << "\t>>> " << P[i] << endl;
    }

    std::cerr << "\t>>> Steps (type steps): " << steps << endl;

    return curve;
}

float bezier(float A, float B, float C, float D, float t) {
    
    float s = 1.0f - t;
    float q = 0.0f;

    q += s * s * s * A;
    q += 3 * t * s * s * B;
    q += 3 * t * t * s * C;
    q += t * t * t * D;

    return q;
}

float bezierTangent(float A, float B, float C, float D, float t) {
    float s = 1.0f - t;
    float q = 0.0f;

    q += (-3.0f) * s * s * A;
    q += (3.0f * s * s - 6.0f * t * s) * B;
    q += (6.0f * t * s - 3.0f * t * t) * C;
    q += 3.0f * t * t * D;

    return q;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        std::cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }


    Curve curve;
    curve.reserve(P.size());

    for (int pointIdx = 0; pointIdx < P.size() - 3; ++pointIdx) {


        for (int it = 0; it < steps; ++it) {
            CurvePoint point;

            float t = static_cast<float>(it) / steps;

            point.V = Vector3f(
                bSpline(P[pointIdx].x(), P[pointIdx + 1].x(), P[pointIdx + 2].x(), P[pointIdx + 3].x(), t),
                bSpline(P[pointIdx].y(), P[pointIdx + 1].y(), P[pointIdx + 2].y(), P[pointIdx + 3].y(), t),
                bSpline(P[pointIdx].z(), P[pointIdx + 1].z(), P[pointIdx + 2].z(), P[pointIdx + 3].z(), t)
            );

            point.T = Vector3f(
                bSplineTangent(P[pointIdx].x(), P[pointIdx + 1].x(), P[pointIdx + 2].x(), P[pointIdx + 3].x(), t),
                bSplineTangent(P[pointIdx].y(), P[pointIdx + 1].y(), P[pointIdx + 2].y(), P[pointIdx + 3].y(), t),
                bSplineTangent(P[pointIdx].z(), P[pointIdx + 1].z(), P[pointIdx + 2].z(), P[pointIdx + 3].z(), t)
            ).normalized();

            if (pointIdx + it == 0) {
                point.N = Vector3f::cross(Vector3f(0, 0, 1), point.T).normalized();
            }
            else {
                point.N = Vector3f::cross(curve[steps * pointIdx + it - 1].B, point.T).normalized();
            }

            point.B = Vector3f::cross(point.T, point.N).normalized();

            curve.push_back(point);
        }
    }

    curve.push_back(curve[0]);


    std::cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    std::cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        std::cerr << "\t>>> " << P[i] << endl;
    }

    std::cerr << "\t>>> Steps (type steps): " << steps << endl;

    return curve;
}

float bSpline(float A, float B, float C, float D, float t) {

    float s = 1.0f - t;
    float q = 0.0f;

    q += s * s * s * A;
    q += (3 * t * t * t - 6 * t * t  + 4) * B;
    q += (-3 * t * t * t + 3 * t * t + 3 * t  + 1) * C;
    q += t * t * t * D;
    q = (1.0f / 6.0f) * q;

    return q;
}

float bSplineTangent(float A, float B, float C, float D, float t) {
    float s = 1.0f - t;
    float q = 0.0f;

    q += -3 * s * s * A;
    q += -3 * (9 * t * t - 12 * t) * B;
    q += (-1.0f / 6.0f) * (-9 * t * t + 6 * t + 3) * C;
    q += 3 * t * t * D;

    q = (1.0f / 6.0f) * q;

    return q;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

