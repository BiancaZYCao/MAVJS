#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <iomanip>

#include "camera.h"
#include "json.h"

using json = nlohmann::json;
namespace fs = boost::filesystem;

Napi::FunctionReference Camera::constructor;

Napi::Object Camera::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Camera", {
    InstanceAccessor("name", &Camera::get_name, nullptr),
    InstanceAccessor("resolution", &Camera::get_resolution, &Camera::set_resolution),
    InstanceAccessor("angle_of_view", &Camera::get_angle_of_view, &Camera::set_angle_of_view),
    InstanceAccessor("img_resolution", &Camera::get_img_resolution, &Camera::set_img_resolution),
    InstanceAccessor("ts", &Camera::get_ts, &Camera::set_ts),
    InstanceAccessor("te", &Camera::get_te, &Camera::set_te),
    InstanceAccessor("overlap", &Camera::get_overlap, &Camera::set_overlap),
    InstanceAccessor("theta", &Camera::get_theta, &Camera::set_theta),
    InstanceAccessor("altitude", &Camera::get_altitude, &Camera::set_altitude),
    InstanceAccessor("aspect_ratio", &Camera::get_aspect_ratio, nullptr),
    StaticMethod("from_json", &Camera::from_json),
    InstanceMethod("save", &Camera::save)
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("Camera", func);
  return exports;
}

Camera::Camera(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Camera>(info)  {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::String name = info[0].As<Napi::String>();
  this->_name = name.Utf8Value();

  Napi::Number resolution = info[1].As<Napi::Number>();
  this->_resolution = resolution.DoubleValue();

  Napi::Number angle_of_view = info[2].As<Napi::Number>();
  this->_angle_of_view = angle_of_view.DoubleValue();

  Napi::Object img_resolution = info[3].As<Napi::Object>();
  this->_img_resolution.x = img_resolution.Get("x").ToNumber().DoubleValue();
  this->_img_resolution.y = img_resolution.Get("y").ToNumber().DoubleValue();

  Napi::Number ts = info[4].As<Napi::Number>();
  this->_ts = ts.DoubleValue();

  Napi::Number te = info[5].As<Napi::Number>();
  this->_te = te.DoubleValue();

  Napi::Object overlap = info[6].As<Napi::Object>();
  this->_overlap.x = overlap.Get("x").ToNumber().DoubleValue();
  this->_overlap.y = overlap.Get("y").ToNumber().DoubleValue();

  Napi::Number theta = info[7].As<Napi::Number>();
  this->_theta = theta.DoubleValue();

  Napi::Number altitude = info[8].As<Napi::Number>();
  this->_altitude = altitude.DoubleValue();

  this->_aspect_ratio = this->_img_resolution.x / this->_img_resolution.y;

}

Napi::Value Camera::get_name(const Napi::CallbackInfo& info) {
  std::string name = this->_name;

  return Napi::String::New(info.Env(), name);
}

Napi::Value Camera::get_resolution(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_resolution);
}

void Camera::set_resolution(const Napi::CallbackInfo &info, const Napi::Value &resolution) {
    Napi::Number arg = resolution.As<Napi::Number>();
    this->_resolution = arg.DoubleValue();
}

Napi::Value Camera::get_angle_of_view(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_angle_of_view);
}

void Camera::set_angle_of_view(const Napi::CallbackInfo &info, const Napi::Value &aov) {
    Napi::Number arg = aov.As<Napi::Number>();
    this->_angle_of_view = arg.DoubleValue();
}

Napi::Value Camera::get_img_resolution(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::Object obj = Napi::Object::New(env);
    obj.Set(Napi::String::New(env, "x"), this->_img_resolution.x);
    obj.Set(Napi::String::New(env, "y"), this->_img_resolution.y);
    return obj;
}

void Camera::set_img_resolution(const Napi::CallbackInfo &info, const Napi::Value &ixy) {
    Napi::Object obj = ixy.As<Napi::Object>();
    this->_img_resolution.x = obj.Get("x").ToNumber().DoubleValue();
    this->_img_resolution.y = obj.Get("y").ToNumber().DoubleValue();
}

Napi::Value Camera::get_ts(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_ts);
}

void Camera::set_ts(const Napi::CallbackInfo &info, const Napi::Value &ts) {
    Napi::Number arg = ts.As<Napi::Number>();
    this->_ts = arg.DoubleValue();
}

Napi::Value Camera::get_te(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_te);
}

void Camera::set_te(const Napi::CallbackInfo &info, const Napi::Value &te) {
    Napi::Number arg = te.As<Napi::Number>();
    this->_te = arg.DoubleValue();
}

Napi::Value Camera::get_overlap(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::Object obj = Napi::Object::New(env);
    obj.Set(Napi::String::New(env, "x"), this->_overlap.x);
    obj.Set(Napi::String::New(env, "y"), this->_overlap.y);
    return obj;
}

void Camera::set_overlap(const Napi::CallbackInfo &info, const Napi::Value &ovxy) {
    Napi::Object obj = ovxy.As<Napi::Object>();
    this->_overlap.x = obj.Get("x").ToNumber().DoubleValue();
    this->_overlap.y = obj.Get("y").ToNumber().DoubleValue();
}

Napi::Value Camera::get_theta(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_theta);
}

void Camera::set_theta(const Napi::CallbackInfo &info, const Napi::Value &theta) {
    Napi::Number arg = theta.As<Napi::Number>();
    this->_theta = arg.DoubleValue();
}

Napi::Value Camera::get_altitude(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_altitude);
}

void Camera::set_altitude(const Napi::CallbackInfo &info, const Napi::Value &altitude) {
    Napi::Number arg = altitude.As<Napi::Number>();
    this->_altitude = arg.DoubleValue();
}

Napi::Value Camera::get_aspect_ratio(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, this->_aspect_ratio);
}

Napi::Object Camera::read_from_json(Napi::Env env, Napi::Value path) {
  Napi::String json_path = path.As<Napi::String>();

  std::ifstream i(json_path.Utf8Value());
  json j;
  i >> j;

  Napi::String name = Napi::String::New(env, j["name"].get<std::string>());
  Napi::Number resolution = Napi::Number::New(env, j["resolution"].get<double>());
  Napi::Number angle_of_view = Napi::Number::New(env, j["angle_of_view"].get<double>());

  Napi::Object img_resolution = Napi::Object::New(env);
  img_resolution.Set(Napi::String::New(env, "x"), j["img_resolution"]["x"].get<double>());
  img_resolution.Set(Napi::String::New(env, "y"), j["img_resolution"]["y"].get<double>());

  Napi::Number ts = Napi::Number::New(env, j["ts"].get<double>());
  Napi::Number te = Napi::Number::New(env, j["te"].get<double>());

  Napi::Object overlap = Napi::Object::New(env);
  overlap.Set(Napi::String::New(env, "x"), j["overlap"]["x"].get<double>());
  overlap.Set(Napi::String::New(env, "y"), j["overlap"]["y"].get<double>());

  Napi::Number theta = Napi::Number::New(env, j["theta"].get<double>());
  Napi::Number altitude = Napi::Number::New(env, j["altitude"].get<double>());

  Napi::Object obj = constructor.New({ name, resolution, angle_of_view, img_resolution, ts, te, overlap, theta, altitude });

  return obj;
}

Napi::Value Camera::from_json(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1 || !info[0].IsString()) {
    throw Napi::TypeError::New(env, "Camera.from_json static method require one argument(`json file path`) as `String`");
  }

  fs::path path_name(info[0].As<Napi::String>().Utf8Value());

  if (fs::is_directory(path_name)) {
    Napi::Array cameras = Napi::Array::New(env);

    fs::directory_iterator end_iter;
    int index = 0;

    for (fs::directory_iterator dir_itr(path_name); dir_itr != end_iter; ++dir_itr) {
      if (dir_itr->path().extension() == ".json") {
        cameras[index] = Camera::read_from_json(env, Napi::String::New(env, dir_itr->path().string()));
      }
    }

    return cameras;
  }

  return Camera::read_from_json(env, info[0]);
}

void Camera::save(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1 || !info[0].IsString()) {
    throw Napi::TypeError::New(env, "Camera save method require one argument(`json file path`) as `String`");
  }

  json j;

  j["name"] = this->_name;
  j["resolution"] = this->_resolution;
  j["angle_of_view"] = this->_angle_of_view;
  j["img_resolution"] = { {"x", this->_img_resolution.x}, {"y", this->_img_resolution.y} };
  j["ts"] = this->_ts;
  j["te"] = this->_te;
  j["overlap"] = { {"x", this->_overlap.x}, {"y", this->_overlap.y} };
  j["theta"] = this->_theta;
  j["altitude"] = this->_altitude;

  Napi::String json_path = info[0].As<Napi::String>();
  std::ofstream o(json_path);
  o << std::setw(4) << j << std::endl;
}
