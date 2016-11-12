<?php

namespace Ecoli;

interface SingletonContract
{
	/**
	 * Register any hook and run initializer routines.
	 *
	 * @return void
	 */
	public function boot();

	/**
	 * Inject core application.
	 *
	 * @return void
	 */
	public function inject($core);

}
