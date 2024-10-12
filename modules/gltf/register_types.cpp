/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "register_types.h"

#include "extensions/gltf_document_extension_convert_importer_mesh.h"
#include "extensions/gltf_document_extension_texture_ktx.h"
#include "extensions/gltf_document_extension_texture_webp.h"
#include "extensions/gltf_spec_gloss.h"
#include "extensions/physics/gltf_document_extension_physics.h"
#include "gltf_document.h"
#include "gltf_state.h"

#define GLTF_REGISTER_DOCUMENT_EXTENSION(m_doc_ext_class) \
	Ref<m_doc_ext_class> extension_##m_doc_ext_class;     \
	extension_##m_doc_ext_class.instantiate();            \
	GLTFDocument::register_gltf_document_extension(extension_##m_doc_ext_class);

void initialize_gltf_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		// glTF API available at runtime.
		GDREGISTER_CLASS(GLTFAccessor);
		GDREGISTER_CLASS(GLTFAnimation);
		GDREGISTER_CLASS(GLTFBufferView);
		GDREGISTER_CLASS(GLTFCamera);
		GDREGISTER_CLASS(GLTFDocument);
		GDREGISTER_CLASS(GLTFDocumentExtension);
		GDREGISTER_CLASS(GLTFDocumentExtensionConvertImporterMesh);
		GDREGISTER_CLASS(GLTFLight);
		GDREGISTER_CLASS(GLTFMesh);
		GDREGISTER_CLASS(GLTFNode);
		GDREGISTER_CLASS(GLTFPhysicsBody);
		GDREGISTER_CLASS(GLTFPhysicsShape);
		GDREGISTER_CLASS(GLTFSkeleton);
		GDREGISTER_CLASS(GLTFSkin);
		GDREGISTER_CLASS(GLTFSpecGloss);
		GDREGISTER_CLASS(GLTFState);
		GDREGISTER_CLASS(GLTFTexture);
		GDREGISTER_CLASS(GLTFTextureSampler);
		// Register GLTFDocumentExtension classes with GLTFDocument.
		// Ensure physics is first in this list so that physics nodes are created before other nodes.
		GLTF_REGISTER_DOCUMENT_EXTENSION(GLTFDocumentExtensionPhysics);
		GLTF_REGISTER_DOCUMENT_EXTENSION(GLTFDocumentExtensionTextureKTX);
		GLTF_REGISTER_DOCUMENT_EXTENSION(GLTFDocumentExtensionTextureWebP);
		bool is_editor = Engine::get_singleton()->is_editor_hint();
		if (!is_editor) {
			GLTF_REGISTER_DOCUMENT_EXTENSION(GLTFDocumentExtensionConvertImporterMesh);
		}
	}
}

void uninitialize_gltf_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GLTFDocument::unregister_all_gltf_document_extensions();
}
