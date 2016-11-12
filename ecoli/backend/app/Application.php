<?php

namespace Ecoli;

class Application
{
	/**
	 * Application name instance.
	 *
	 * @var Class
	 */
	private $core;

	/**
	 * Application config.
	 *
	 * @var Array
	 */
	public $config;

	/**
	 * Define the application's command schedule.
	 *
	 * @param  string  $name
	 * @return void
	 */
	public function __construct($name)
	{
		$this->core = new $name;
	}

	/**
	 * Define the application's singleton classes.
	 *
	 * @param  Array  $classes
	 * @return void
	 */
	public function singleton(Array $classes)
	{
		foreach ($classes as $class) {
			$object = $class::getInstance();
			$object->inject($this);
			$object->boot();
		}
	}

	/**
	 * Load core module.
	 *
	 * @return void
	 */
	public function loadCore()
	{
		$this->config = $this->core->getConfig();
		$this->core->registerRoutes();
	}

	/**
	 * Handle incomming requests.
	 *
	 * @param  Ecoli\Request  $request
	 * @return Ecoli\Response
	 */
	public function handle(Request $request)
	{
		$response = new Response;

		$router = Router::getInstance();
		$dest = $router->match($request);
		if (is_null($dest)) {
			return $response->makeNotFound()->data('Not Found');
		}

		$controller = new $dest[0];

		return $controller->{$dest[1]}();
	}

	/**
	 * Terminate the application.
	 *
	 * @return void
	 */
	public function terminate()
	{
		exit();
	}

}
