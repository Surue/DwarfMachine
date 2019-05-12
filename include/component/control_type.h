/*
MIT License

Copyright (c) 2019 Nicolas Schneider

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CONTROLLER_TYPE_H
#define CONTROLLER_TYPE_H
#include <component/component.h>

namespace dm {
struct ControllerType final : ComponentBase
{
	enum class ControllerTypeEnum {
		CAMERA_EDITOR
	};

	ControllerTypeEnum type = ControllerTypeEnum::CAMERA_EDITOR;
	//TODO rajouter plus de type de contrôls (FPS, TOP_DOWN)
};

class ControllerTypeManager final : public ComponentBaseManager<ControllerType>
{
public:
	ControllerTypeManager();

	void Init() override;

	void Update() override;

	ControllerType* CreateComponent(const Entity entity) override;

	void DestroyComponent(Entity entity) override;

	void OnDrawInspector(Entity entity) override;
};
}
#endif CONTROLLER_TYPE_H