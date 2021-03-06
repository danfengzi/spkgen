#include "Path.h"
#include <assert.h>
#include <math.h>

//------------------------------------------------------------------------------
// Path impl
//------------------------------------------------------------------------------

Path::Path(PathType splineType, unsigned int keyCount) :
    _spline(nullptr),
    _loopMode(ELM_LAST)
{
    ///switch(splineType)
    //{
    //case EPT_CONSTANT:    _spline = new Spline;  break;
    //case EPT_LINEAR:      _spline = new Spline;    break;
    //case EPT_CUBIC:       _spline = new Spline;     break;
    //case EPT_AKIMA:       _spline = new Spline;     break;
    //case EPT_BESSEL:      _spline = new Spline;    break;
    //case EPT_PCHIP:       _spline = new Spline;     break;
    //case EPT_QUINTIC:     _spline = new Spline;   break;
    //default: assert(0);
    //}
    _spline = new Spline(splineType);

    //_times.resize(keyCount);
    //_values.resize(keyCount);
}

Path::~Path()
{
    if(_spline)
        delete _spline;
}

void Path::addKey(double keyTime, double keyValue)
{
    assert(keyTime >= 0.0);

    ///_insertKey(keyTime, keyValue);
    _times.push_back(keyTime);
    _values.push_back(keyValue);
}

void Path::_insertKey(double keyTime, double keyValue)
{
    for (unsigned int i=0; i<_times.size(); i++)
    {
        if (_times[i] > keyTime)
        {
            _times.insert(_times.begin()+i, keyTime);
            _values.insert(_values.begin()+i, keyValue);
            return;
        }
    }

    // no key found => append
    _times.push_back(keyTime);
    _values.push_back(keyValue);
}

unsigned int Path::getKeyCount() const
{
    return _times.size();
}

const PathKey Path::getKeyByIndex(unsigned int index) const
{
    assert(index < _times.size());

    PathKey key;
    key.time = _times.at(index);
    key.value = _values.at(index);
    return key;
}

double Path::getStartTime() const
{
    if (_times.size() == 0)
        return 0.0;

    return _times.front();
}

double Path::getEndTime() const
{
    if (_times.size() == 0)
        return 0.0f;

    return _times.back();
}

void Path::clear()
{
    _times.clear();
    _values.clear();
}

void Path::setAtIndex(unsigned index, double time, double value)
{
    assert(index < _times.size());
    _times[index] = time;
    _values[index] = value;
}

void Path::build()
{
    _spline->clear();
    _spline->build(_times.data(), _values.data(), _times.size());
}

double Path::evaluate(double time) const
{
    if (!_times.size())
        return 0.0;

    const double minTime = _times.front();
    const double maxTime = _times.back();

    const double firstValue = _values.front();
    const double lastValue = _values.back();

    if (_loopMode == ELM_LOOP)
        time = fmodf(time - minTime, maxTime - minTime);

    if (time <= minTime || _times.size() == 1) // time before first key?
        return (_loopMode == ELM_ZERO ? 0.0 : firstValue);
    else if (time >= maxTime) //  time after last key?
        return (_loopMode == ELM_ZERO ? 0.0 : lastValue);
    else
        return _spline->eval(time);
}

void Path::setLoopMode(PathLoopMode loopMode)
{
    _loopMode = loopMode;
}


//------------------------------------------------------------------------------
// Path4 impl
//------------------------------------------------------------------------------

Path4::Float4::Float4(float x, float y, float z, float w) :
    x(x), y(y), z(z), w(w)
{
}

Path4::Path4(PathType splineType)
{
    /*for (int i=0; i<4; i++)
    {
        _subPaths[i]
    }*/
}

Path4::Float4 Path4::evaluate(double time) const
{
    return Path4::Float4(_subPaths[0].evaluate(time),
                    _subPaths[1].evaluate(time),
                    _subPaths[2].evaluate(time),
                    _subPaths[3].evaluate(time));
}

const Path& Path4::getSubPath(unsigned int index) const
{
    assert(index < 4);
    return _subPaths[index];
}

Path& Path4::getSubPath(unsigned int index)
{
    assert(index < 4);
    return _subPaths[index];
}
