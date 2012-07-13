/*
 * AndroidApplication.h
 *
 *  Created on: 18/05/2012
 *      Author: objective
 */

#ifndef ANDROIDAPPLICATION_H_
#define ANDROIDAPPLICATION_H_

#include "Application.h"
#include "android/AndroidGraphicManager.h"
#include "System.h"
#include "Shader.h"

namespace engine {

	struct saved_state {
		float angle;
		int32_t x;
		int32_t y;
	};

	class AndroidApplication : public Application {
	public:
		AndroidApplication(struct android_app* app) :
				app(app) {
			display = NULL;

			app->userData = this;
			app->onAppCmd = engine_handle_cmd;
			app->onInputEvent = engine_handle_input;

			// Prepare to monitor accelerometer
			sensorManager = ASensorManager_getInstance();
			accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
			sensorEventQueue = ASensorManager_createEventQueue(sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);

			if(app->savedState != NULL) {
				// We are starting with a previous saved state; restore from it.
				state = *(saved_state*) app->savedState;
			}
		}
	protected:
		virtual void render() {
			if(display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
				return;
			}

			glClearColor(1, 0, 1, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

//	    graphicManager->bindShader(shader);
//
			const engine::math::Vector2 gTriangleVertices[] = { engine::math::Vector2(0.0f, 0.5f), engine::math::Vector2(
					-0.5f, -0.5f), engine::math::Vector2(0.5f, -0.5f) };
//
//	    unsigned short index[] = {
//	   		 0, 1, 2
//	    };
//
//	    position->setValue(gTriangleVertices, 3);
//
//	    graphicManager->drawIndex(index, 1);
//
//	    graphicManager->unbindShader();

			shader->bind();

			position->setValue(gTriangleVertices, 3);
			static float angle = 0;
			angle += 0.01;
			engine::math::AxisAngle axisAngle(angle, engine::math::Vector3(0, 0, 1));
			matrix->setValue(engine::math::Matrix4::rotationMatrix(engine::math::Quaternion(axisAngle)));

			glDrawArrays(GL_TRIANGLES, 0, 3);

			shader->unbind();
		}

		virtual bool processMessages() {
			// Read all pending events.
			int ident;
			int events;
			struct android_poll_source* source;

			// If not animating, we will block forever waiting for events.
			// If animating, we loop until all events are read, then continue
			// to draw the next frame of animation.
			while((ident = ALooper_pollAll(0, NULL, &events, (void**) &source)) >= 0) {

				// Process this event.
				if(source != NULL) {
					source->process(app, source);
				}

				// If a sensor has data, process it now.
				if(ident == LOOPER_ID_USER) {
					if(accelerometerSensor != NULL) {
						ASensorEvent event;
						while(ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
							//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
						}
					}
				}

				// Check if we are exiting.
				if(app->destroyRequested != 0) {
					engine_terminate_display();
					quit();
					return false;
				}
			}

			return true;
		}

		virtual void swapBuffers() {
			if(display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
				return;
			}

			eglSwapBuffers(display, surface);
		}

		int engine_initialize_display() {
			// initialize OpenGL ES and EGL

			/*
			 * Here specify the attributes of the desired configuration.
			 * Below, we select an EGLConfig with at least 8 bits per color
			 * component compatible with on-screen windows
			 */
			const EGLint attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE,
					8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE };
			EGLint dummy, format;
			EGLint numConfigs;
			EGLConfig config;

			animating = 0;
			display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			eglInitialize(display, 0, 0);

			/* Here, the application chooses the configuration it desires. In this
			 * sample, we have a very simplified selection process, where we pick
			 * the first EGLConfig that matches our criteria */
			eglChooseConfig(display, attribs, &config, 1, &numConfigs);

			/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
			 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
			 * As soon as we picked a EGLConfig, we can safely reconfigure the
			 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
			eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

			ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

			surface = eglCreateWindowSurface(display, config, app->window, NULL);
			int attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
			context = eglCreateContext(display, config, NULL, attrib_list);

			if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
				LOGW("Unable to eglMakeCurrent");
				return -1;
			}

			eglQuerySurface(display, surface, EGL_WIDTH, &width);
			eglQuerySurface(display, surface, EGL_HEIGHT, &height);

			state.angle = 0;

			// Initialize GL state.
			//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
			glEnable (GL_CULL_FACE);
			//glShadeModel(GL_SMOOTH);
			glDisable (GL_DEPTH_TEST);

			graphicManager = new engine::AndroidGraphicManager;

			static const char gVertexShader[] = "uniform mat4 matrix;\n"
					"attribute vec4 vPosition;\n"
					"void main() {\n"
					"  gl_Position = matrix * vPosition;\n"
					"}\n";

			static const char gFragmentShader[] = "precision mediump float;\n"
					"void main() {\n"
					"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
					"}\n";

			Source* vertex = new Source(gVertexShader);
			Source* fragment = new Source(gFragmentShader);

			shader = new Shader(vertex, fragment, 0);
			shader->link();

			position = shader->getAttribute("vPosition");
			matrix = shader->getConstant("matrix");

			return 0;
		}

		void engine_terminate_display() {
			if(display != EGL_NO_DISPLAY) {
				eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

				if(context != EGL_NO_CONTEXT) {
					eglDestroyContext(display, context);
				}

				if(surface != EGL_NO_SURFACE) {
					eglDestroySurface(display, surface);
				}

				eglTerminate(display);
			}

			animating = 0;
			display = EGL_NO_DISPLAY;
			context = EGL_NO_CONTEXT;
			surface = EGL_NO_SURFACE;
		}

		static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
			AndroidApplication* engine = (AndroidApplication*) app->userData;

			switch(cmd) {
			case APP_CMD_SAVE_STATE:
				// The system has asked us to save our current state.  Do so.
				engine->app->savedState = malloc(sizeof(struct saved_state));
				*((saved_state*) engine->app->savedState) = engine->state;
				engine->app->savedStateSize = sizeof(struct saved_state);
				break;

			case APP_CMD_INIT_WINDOW:
				// The window is being shown, get it ready.
				if(engine->app->window != NULL) {
					engine->engine_initialize_display();
					engine->render();
				}
				break;

			case APP_CMD_TERM_WINDOW:
				// The window is being hidden or closed, clean it up.
				engine->engine_terminate_display();
				break;

			case APP_CMD_GAINED_FOCUS:
				// When our app gains focus, we start monitoring the accelerometer.
				if(engine->accelerometerSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
					// We'd like to get 60 events per second (in us).
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometerSensor,
							(1000L / 60) * 1000);
				}
				break;

			case APP_CMD_LOST_FOCUS:
				// When our app loses focus, we stop monitoring the accelerometer.
				// This is to avoid consuming battery while not being used.
				if(engine->accelerometerSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
				}

				// Also stop animating.
				engine->animating = 0;
				engine->render();
				break;
			}
		}

		static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
			AndroidApplication* engine = (AndroidApplication*) app->userData;

			if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
				engine->animating = 1;
				engine->state.x = AMotionEvent_getX(event, 0);
				engine->state.y = AMotionEvent_getY(event, 0);
				return 1;
			}

			return 0;
		}

		virtual void createApplication() {
		}

		virtual void destroyApplication() {
		}
	private:
		android_app* app;

		ASensorManager* sensorManager;
		const ASensor* accelerometerSensor;
		ASensorEventQueue* sensorEventQueue;

		int animating;
		EGLDisplay display;
		EGLSurface surface;
		EGLContext context;
		int32_t width;
		int32_t height;
		saved_state state;

		Shader* shader;
		Attribute* position;
		Constant* matrix;
		AndroidGraphicManager* graphicManager;
	};
}

#endif /* ANDROIDAPPLICATION_H_ */
