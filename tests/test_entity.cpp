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

#include <gtest/gtest.h>

#include <engine/engine.h>
#include <engine/entity.h>

TEST(Entity, CreateEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	const auto e1 = entityManager->CreateEntity();
	const auto e2 = entityManager->CreateEntity();

	std::cout << "e0 = " << e0 << "\n";
	std::cout << "e1 = " << e1 << "\n";
	std::cout << "e2 = " << e2 << "\n";

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}

	system("pause");
}

TEST(Entity, CreateMultipleEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	for(size_t i = 0; i < 100; i++)
	{
		const auto e = entityManager->CreateEntity();
		std::cout << "e" << i << " = " << e << "\n";
	}

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}

	system("pause");
}

TEST(Entity, CreateAndDestroyEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	try
	{
		const auto e0 = entityManager->CreateEntity();
		std::cout << "e0 = " << e0 << "\n";
		const auto e1 = entityManager->CreateEntity();
		std::cout << "e1 = " << e1 << "\n";
		std::cout << "destroy e1\n";
		entityManager->DestroyEntity(e1);
		const auto e2 = entityManager->CreateEntity();
		std::cout << "e2 = " << e2 << "\n";

		entityManager->DestroyEntity(199);
		
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}