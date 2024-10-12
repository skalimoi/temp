/**************************************************************************/
/*  path_2d.cpp                                                           */
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

#include "path_2d.h"

#include "core/math/geometry_2d.h"
#include "scene/main/timer.h"

void Path2D::_curve_changed() {
	if (!is_inside_tree()) {
		return;
	}

	if (!Engine::get_singleton()->is_editor_hint() && !get_tree()->is_debugging_paths_hint()) {
		return;
	}

	queue_redraw();
	for (int i = 0; i < get_child_count(); i++) {
		PathFollow2D *follow = Object::cast_to<PathFollow2D>(get_child(i));
		if (follow) {
			follow->path_changed();
		}
	}
}

void Path2D::set_curve(const Ref<Curve2D> &p_curve) {
	if (curve.is_valid()) {
		curve->disconnect_changed(callable_mp(this, &Path2D::_curve_changed));
	}

	curve = p_curve;

	if (curve.is_valid()) {
		curve->connect_changed(callable_mp(this, &Path2D::_curve_changed));
	}

	_curve_changed();
}

Ref<Curve2D> Path2D::get_curve() const {
	return curve;
}


/////////////////////////////////////////////////////////////////////////////////

void PathFollow2D::path_changed() {
	if (update_timer && !update_timer->is_stopped()) {
		update_timer->start();
	} else {
		_update_transform();
	}
}

void PathFollow2D::_update_transform() {
	if (!path) {
		return;
	}

	Ref<Curve2D> c = path->get_curve();
	if (!c.is_valid()) {
		return;
	}

	real_t path_length = c->get_baked_length();
	if (path_length == 0) {
		return;
	}

	if (rotates) {
		Transform2D xform = c->sample_baked_with_rotation(progress, cubic);
		xform.translate_local(h_offset, v_offset);
		set_rotation(xform[0].angle());
		set_position(xform[2]);
	} else {
		Vector2 pos = c->sample_baked(progress, cubic);
		pos.x += h_offset;
		pos.y += v_offset;
		set_position(pos);
	}
}

void PathFollow2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (Engine::get_singleton()->is_editor_hint()) {
				update_timer = memnew(Timer);
				update_timer->set_wait_time(0.2);
				update_timer->set_one_shot(true);
				update_timer->connect("timeout", callable_mp(this, &PathFollow2D::_update_transform));
				add_child(update_timer, false, Node::INTERNAL_MODE_BACK);
			}
		} break;

		case NOTIFICATION_ENTER_TREE: {
			path = Object::cast_to<Path2D>(get_parent());
			if (path) {
				_update_transform();
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			path = nullptr;
		} break;
	}
}

void PathFollow2D::set_cubic_interpolation_enabled(bool p_enabled) {
	cubic = p_enabled;
}

bool PathFollow2D::is_cubic_interpolation_enabled() const {
	return cubic;
}

void PathFollow2D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "offset") {
		real_t max = 10000.0;
		if (path && path->get_curve().is_valid()) {
			max = path->get_curve()->get_baked_length();
		}

		p_property.hint_string = "0," + rtos(max) + ",0.01,or_less,or_greater";
	}
}

PackedStringArray PathFollow2D::get_configuration_warnings() const {
	PackedStringArray warnings = Node::get_configuration_warnings();

	if (is_visible_in_tree() && is_inside_tree()) {
		if (!Object::cast_to<Path2D>(get_parent())) {
			warnings.push_back(RTR("PathFollow2D only works when set as a child of a Path2D node."));
		}
	}

	return warnings;
}

void PathFollow2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_progress", "progress"), &PathFollow2D::set_progress);
	ClassDB::bind_method(D_METHOD("get_progress"), &PathFollow2D::get_progress);

	ClassDB::bind_method(D_METHOD("set_h_offset", "h_offset"), &PathFollow2D::set_h_offset);
	ClassDB::bind_method(D_METHOD("get_h_offset"), &PathFollow2D::get_h_offset);

	ClassDB::bind_method(D_METHOD("set_v_offset", "v_offset"), &PathFollow2D::set_v_offset);
	ClassDB::bind_method(D_METHOD("get_v_offset"), &PathFollow2D::get_v_offset);

	ClassDB::bind_method(D_METHOD("set_progress_ratio", "ratio"), &PathFollow2D::set_progress_ratio);
	ClassDB::bind_method(D_METHOD("get_progress_ratio"), &PathFollow2D::get_progress_ratio);

	ClassDB::bind_method(D_METHOD("set_rotates", "enabled"), &PathFollow2D::set_rotation_enabled);
	ClassDB::bind_method(D_METHOD("is_rotating"), &PathFollow2D::is_rotation_enabled);

	ClassDB::bind_method(D_METHOD("set_cubic_interpolation", "enabled"), &PathFollow2D::set_cubic_interpolation_enabled);
	ClassDB::bind_method(D_METHOD("get_cubic_interpolation"), &PathFollow2D::is_cubic_interpolation_enabled);

	ClassDB::bind_method(D_METHOD("set_loop", "loop"), &PathFollow2D::set_loop);
	ClassDB::bind_method(D_METHOD("has_loop"), &PathFollow2D::has_loop);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "progress", PROPERTY_HINT_RANGE, "0,10000,0.01,or_less,or_greater,suffix:px"), "set_progress", "get_progress");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "progress_ratio", PROPERTY_HINT_RANGE, "0,1,0.0001,or_less,or_greater", PROPERTY_USAGE_EDITOR), "set_progress_ratio", "get_progress_ratio");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "h_offset"), "set_h_offset", "get_h_offset");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "v_offset"), "set_v_offset", "get_v_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rotates"), "set_rotates", "is_rotating");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "cubic_interp"), "set_cubic_interpolation", "get_cubic_interpolation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop"), "set_loop", "has_loop");
}

void PathFollow2D::set_progress(real_t p_progress) {
	ERR_FAIL_COND(!isfinite(p_progress));
	progress = p_progress;
	if (path) {
		if (path->get_curve().is_valid()) {
			real_t path_length = path->get_curve()->get_baked_length();

			if (loop && path_length) {
				progress = Math::fposmod(progress, path_length);
				if (!Math::is_zero_approx(p_progress) && Math::is_zero_approx(progress)) {
					progress = path_length;
				}
			} else {
				progress = CLAMP(progress, 0, path_length);
			}
		}

		_update_transform();
	}
}

void PathFollow2D::set_h_offset(real_t p_h_offset) {
	h_offset = p_h_offset;
	if (path) {
		_update_transform();
	}
}

real_t PathFollow2D::get_h_offset() const {
	return h_offset;
}

void PathFollow2D::set_v_offset(real_t p_v_offset) {
	v_offset = p_v_offset;
	if (path) {
		_update_transform();
	}
}

real_t PathFollow2D::get_v_offset() const {
	return v_offset;
}

real_t PathFollow2D::get_progress() const {
	return progress;
}

void PathFollow2D::set_progress_ratio(real_t p_ratio) {
	if (path && path->get_curve().is_valid() && path->get_curve()->get_baked_length()) {
		set_progress(p_ratio * path->get_curve()->get_baked_length());
	}
}

real_t PathFollow2D::get_progress_ratio() const {
	if (path && path->get_curve().is_valid() && path->get_curve()->get_baked_length()) {
		return get_progress() / path->get_curve()->get_baked_length();
	} else {
		return 0;
	}
}

void PathFollow2D::set_rotation_enabled(bool p_enabled) {
	rotates = p_enabled;
	_update_transform();
}

bool PathFollow2D::is_rotation_enabled() const {
	return rotates;
}

void PathFollow2D::set_loop(bool p_loop) {
	loop = p_loop;
}

bool PathFollow2D::has_loop() const {
	return loop;
}
