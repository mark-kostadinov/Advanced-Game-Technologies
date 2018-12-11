#pragma once
#include "../../Common/RendererBase.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"


#ifdef _WIN32
#include "windows.h"
#endif

#include <string>
#include <vector>

namespace NCL {
	class MeshGeometry;
	namespace Rendering {
		class ShaderBase;
		class TextureBase;

		class OGLMesh;
		class OGLShader;

		class SimpleFont;

		class OGLRenderer : public RendererBase
		{
		public:
			friend class OGLRenderer;
			OGLRenderer(Window& w);
			~OGLRenderer();

			void OnWindowResize(int w, int h)	override;

			void DrawString(const std::string& text, const Vector2&pos, const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

			void DrawLine(const Vector3& start, const Vector3& end, const Vector4& colour);

		protected:			
			void BeginFrame()	override;
			void RenderFrame()	override;
			void EndFrame()		override;

			void DrawDebugData();
			void DrawDebugStrings();
			void DrawDebugLines();
			virtual void SetupDebugMatrix(OGLShader*s) {
			}

			void BindShader(ShaderBase*s);
			void BindTextureToShader(const TextureBase*t, const std::string& uniform, int texUnit) const;
			void BindMesh(MeshGeometry*m);
			void DrawBoundMesh(int subLayer = 0, int numInstances = 1);
#ifdef _WIN32
			void InitWithWin32(Window& w);
			void DestroyWithWin32();
			HDC		deviceContext;		//...Device context?
			HGLRC	renderContext;		//Permanent Rendering Context
#endif
		private:
			struct DebugString {
				Maths::Vector4 colour;
				Maths::Vector2	ndcPos;
				float			size;
				std::string		text;
			};

			struct DebugLine {
				Maths::Vector3 start;
				Maths::Vector3 end;
				Maths::Vector4 colour;
			};

			OGLMesh*	boundMesh;
			OGLShader*	boundShader;

			OGLShader*  debugShader;
			SimpleFont* font;
			std::vector<DebugString>	debugStrings;
			std::vector<DebugLine>		debugLines;
		};
	}
}